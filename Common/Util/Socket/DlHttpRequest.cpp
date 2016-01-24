//+
//	DlHttpRequest.cpp
//
//	Copyright © 2001 David C. Salmon. All Rights Reserved
//
//	Contains simple http request/response class for mac.
//-

//---------------------------------- Includes ----------------------------------

#include "DlPlatform.h"
#include "DlHttpRequest.h"
#include "DlBufferedSocket.h"
#include "DlSimpException.h"
#include "DlConnectorSocket.h"
#include "DlStringUtil.h"
#include <ctype.h>
#include <stdexcept>
#include <cstdlib>
#include <cstdio>

#include <unistd.h>

using namespace std;

//---------------------------------- Declares ----------------------------------

// response format
const char* const kResponse = "HTTP/%d.%d %d ";


//----------------------------------------------------------------------------------------
//  DlHttpRequest::DlHttpRequest                                              constructor
//
//      construct an http request.
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlHttpRequest::DlHttpRequest()
	: _contentLength(0)
	, _responseCode(0)
{
}

//----------------------------------------------------------------------------------------
//  DlHttpRequest::~DlHttpRequest                                              destructor
//
//      destruct request.
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlHttpRequest::~DlHttpRequest()
{
}

//----------------------------------------------------------------------------------------
//  DlHttpRequest::Reset
//
//      reset the request object.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlHttpRequest::Reset()
{
	_requestHeaders.clear();
	_responseHeaders.clear();
	_contentLength = 0;
	_responseCode = 0;
	_responseBuffer.clear();
}

//----------------------------------------------------------------------------------------
//  DlHttpRequest::AddHeader
//
//      add the specified request header.
//
//  const char* name   -> the header name.
//  const char* value  -> the header value.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlHttpRequest::AddHeader(const char* name, const char* value)
{
	_requestHeaders[name] = value;
}

//----------------------------------------------------------------------------------------
//  DlHttpRequest::Transact
//
//      run the transaction. return false if the response code is not 200.
//
//  const char* resource   -> the path and query for the resource.
//  const char* hostname   -> the host name.
//  DlInt32 port           -> the port to connect on.
//
//  returns bool           <- true if successful.
//----------------------------------------------------------------------------------------
bool
DlHttpRequest::Transact(const char* resource, const char* hostname, DlInt32 port)
{
	DlSocket conn(DlConnectorSocket().Connect(hostname, port));
	DlBufferedSocket sock(conn);
	return Transact(resource, sock);
}

//----------------------------------------------------------------------------------------
//  DlHttpRequest::GetResponse
//
//      return the response as a block of bytes. return nullptr if there is no response.
//
//  DlInt32 & len          -> the byte count.
//
//  returns const void *   <- the block of data.
//----------------------------------------------------------------------------------------
const void *
DlHttpRequest::GetResponse(DlInt32 & len)
{
	len = _contentLength;
	
	if (_responseBuffer.size() == 0)
		return nullptr;
	
	// if more than one block was read in, then coalesce the blocks into
	//	a single one.
	if (_responseBuffer.size() > 1)
	{
		ResponseBlock contents(_contentLength);
		DlUInt32 ofs = 0;
		for (auto& elem : _responseBuffer)
		{
			std::memcpy(contents._block.get() + ofs, elem._block.get(), elem._len);
			ofs += elem._len;
		}
		
		_responseBuffer.clear();
		
		_responseBuffer.push_back(std::move(contents));
	}
	
	return _responseBuffer[0]._block.get();
}

//----------------------------------------------------------------------------------------
//  DlHttpRequest::readBlock
//
//      read in a block of data. In the case of chunked transfer, we want to check to
//		see that we actually got the amount of data we expected.
//
//  DlBufferedSocket& s    -> the socket
//  DlUInt32 size          -> the block size
//  bool checkSize         -> if true, check the size
//
//  returns DlUInt32       <- 
//----------------------------------------------------------------------------------------
DlUInt32
DlHttpRequest::readBlock(DlBufferedSocket& s, DlUInt32 size, bool checkSize)
{
	ResponseBlock block(size);
	DlUInt32 actSize = s.ReadBlock(block._block.get(), size);
	
	if (actSize != size)
	{
		if (checkSize)
			throw DlSimpException("DlHttpRequest: available size less than chunk size.");
		block._len = size;
	}
	
	_responseBuffer.push_back(std::move(block));
	
	return actSize;
}


//----------------------------------------------------------------------------------------
//  DlHttpRequest::Transact
//
//      run the transaction.
//
//  const char* resource   -> 
//  DlBufferedSocket& s    -> 
//
//  returns bool           <- 
//----------------------------------------------------------------------------------------
bool
DlHttpRequest::Transact(const char* resource, DlBufferedSocket& s)
{
	string	httpLine = "GET ";
	httpLine += resource;
	httpLine += " HTTP/1.1";
	
	s.WriteLine(httpLine);
	
	httpLine = "Host: ";
	httpLine += s.GetSocket().GetHostName();
	s.WriteLine(httpLine);
	
	const_iterator i = _requestHeaders.begin();
	while(i != _requestHeaders.end())
	{
		httpLine = i->first;
		httpLine += ": ";
		httpLine += i->second;
		s.WriteLine(httpLine);
		i++;
	}

	if (!GetRequestHeader("connection", httpLine))
		s.WriteLine("Connection: close");

	s.WriteLine("");

	s.Flush();
	
	int	majVers;
	int	minVers;
	int	code = 0;
	
	if (s.ReadLine(httpLine)) {
		int count = sscanf(httpLine.c_str(), kResponse, &majVers, &minVers, &code);
		if (count != 3) {
			_responseCode = 500;
			return false;
		}
	}
	
	_responseCode = code;
	_responseBuffer.clear();
	
	if (code < 500) {
		
		_contentLength = 0;
	
		while (s.ReadLine(httpLine)) {
			if (httpLine.length() == 0)
				break;					//	end of header section
			
			addResponseHeader(httpLine);
			//	first line looks line 
		}
		
		//	now read the body
		if (GetResponseHeader("content-length", httpLine))
			_contentLength = atoi(httpLine.c_str());

		if (_contentLength) {
			_contentLength = readBlock(s, _contentLength);
		}
		else if (DlStrICmp(getResponseHeader("transfer-encoding"), "chunked") == 0)
		{
			// handle chunked transfer encoding.
			while (true)
			{
				if (!s.ReadLine(httpLine))
					throw DlSimpException("DlHttpRequest: invalid chunked transfer value");
				char* end = 0;
				DlUInt32 chunkSize = strtoul(httpLine.c_str(), &end, 16);
				if (*end != 0)
					throw DlSimpException("DlHttpRequest: invalid chunked transfer value (%s)",
										  httpLine.c_str());
				
				if (chunkSize == 0)
					break;
				
				_contentLength += readBlock(s, chunkSize, true);
			}
		}
		else
		{
			DlUInt32	bufLen = 32768;
			
			while (true) {
				
				DlUInt32 actLen = readBlock(s, bufLen);
				if (actLen == 0)
					break;
				
				_contentLength += actLen;
			}
		}
	}

	return code == 200;
}

//----------------------------------------------------------------------------------------
//  DlHttpRequest::addResponseHeader
//
//      add header from response to response header list.
//
//  const string & s   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlHttpRequest::addResponseHeader(const string & s)
{
	int pos = s.find(':', 0);
	int endPos = pos;
	while(true)
	{
		int c = s[endPos - 1];
		if (!isspace(c))
			break;
		endPos--;
	}
	string key = s.substr(0, endPos);
	
	while(true)
	{
		int c = s[pos + 1];
		if (!isspace(c))
			break;
		pos++;
	}
	
	_responseHeaders[key] = s.substr(pos + 1, s.length());
}

//----------------------------------------------------------------------------------------
//  DlHttpRequest::GetRequestHeader
//
//      return a request header.
//
//  const char* name   -> 
//  string & s         -> 
//
//  returns bool       <- 
//----------------------------------------------------------------------------------------
bool
DlHttpRequest::GetRequestHeader(const char* name, string & s) const
{
	const_iterator i = _requestHeaders.find(name);
	if (i != _requestHeaders.end()) {
		s = i->second;
		return true;
	}
	
	return false;
}

//----------------------------------------------------------------------------------------
//  DlHttpRequest::GetResponseHeader
//
//      return a response header.
//
//  const char* name   -> 
//  string & s         -> 
//
//  returns bool       <- 
//----------------------------------------------------------------------------------------
bool
DlHttpRequest::GetResponseHeader(const char* name, string & s) const
{
	const_iterator i = _responseHeaders.find(name);
	if (i != _responseHeaders.end()) {
		s = i->second;
		return true;
	}
	
	return false;
}

//----------------------------------------------------------------------------------------
//  DlHttpKeyCompare::operator()
//
//      compare keys for map in a case insensitive way.
//
//  const string & s1  -> 
//  const string & s2  -> 
//
//  returns int        <- 
//----------------------------------------------------------------------------------------
int
DlHttpKeyCompare::operator() (const string & s1, const string & s2) const
{
	return DlStrICmp(s1.c_str(), s2.c_str()) < 0;
}

//	eof
