//+
//	DlHttpRequest.h
//
//	Copyright © 2001 David C. Salmon. All Rights Reserved
//
//	Contains simple http request/response class for mac.
//-

#ifndef _H_DlHttpRequest
#define _H_DlHttpRequest

//---------------------------------- Includes ----------------------------------

#include <map>
#include <string>
#include <vector>
#include <memory>

//---------------------------------- Declares ----------------------------------

class DlHttpKeyCompare
{
public:
	int operator() (const std::string & s1, const std::string & s2) const;
};

class DlBufferedSocket;

class DlHttpRequest {

	struct ResponseBlock {
		
		ResponseBlock(DlUInt32 len)
			: _len(len), _block(NEW DlUInt8[len]) {}

		ResponseBlock(ResponseBlock&& b)
			: _len(b._len), _block(std::move(b._block)) {}
		
		DlUInt32 _len;
		std::unique_ptr<DlUInt8[]> _block;
	};
	
	typedef std::string 				keytype;
	typedef std::string					valuetype;
	typedef std::map<keytype,valuetype, DlHttpKeyCompare>	maptype;
	typedef maptype::iterator			iterator;
	typedef maptype::const_iterator		const_iterator;
	
	typedef std::vector<ResponseBlock>	ResponseList;
	
public:

	DlHttpRequest();
	~DlHttpRequest();

	bool 		Transact(const char* resource, const char* hostName, DlInt32 port = 80);
	bool 		Transact(const char* resource, DlBufferedSocket& s);

	void 		AddHeader(const char * name, const char * value);	
	bool 		GetResponseHeader(const char * name, std::string & s) const;
	bool 		GetRequestHeader(const char * name, std::string & s) const;

	const void*	GetResponse(DlInt32 & len);

	DlInt32		GetResponseCode() const;

	void		Reset();

private:
    DlHttpRequest(const DlHttpRequest& r);
    DlHttpRequest& operator=(const DlHttpRequest& r);
        
	void		addResponseHeader(const std::string & s);
	const char* getResponseHeader(const char * name);

	DlUInt32	readBlock(DlBufferedSocket& s, DlUInt32 size, bool checkSize = false);
	
	ResponseList _responseBuffer;
	DlInt32		_contentLength;
	DlInt32		_responseCode;
	
	maptype		_requestHeaders;
	maptype 	_responseHeaders;
	
};

//---------------------------------- Inlines -----------------------------------


inline DlInt32
DlHttpRequest::GetResponseCode() const 
{
	return _responseCode;
}

inline const char*
DlHttpRequest::getResponseHeader(const char * name)
{
	auto loc =_responseHeaders.find(name);
	return loc != _responseHeaders.end() ? loc->second.c_str() : nullptr;
}


#endif // _H_DlHttpRequest


