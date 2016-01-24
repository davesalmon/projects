//+
//	DlSocket.h
//
//	Copyright © 2001 David C. Salmon. All Rights Reserved
//
//	Contains simple socket class for mac.
//-

#ifndef _H_DlSocket
#define _H_DlSocket

//---------------------------------- Includes ----------------------------------

#include "DlTypes.h"
#include <OpenTransport.h>
#include <OpenTransportProviders.h>

#include <string>

//---------------------------------- Defines -----------------------------------

const std::size_t	kBufferSize = 1024;
const DlInt32 DlMessageIdle = 6000;

//---------------------------------- Declares ----------------------------------

class DlListener;

typedef struct SockContext
{
	const char * host;
	LListener * l;
}	SockContext;

class DlSocket
{
public:
	DlSocket(const char* host, LListener * l = 0);
	~DlSocket();
	
	const std::string&	GetHostName() const;
	
	int	 				GetChar();
	void 				UngetChar();
	bool 				ReadLine(std::string & line);
	DlInt32 			ReadBuffer(void * buf, DlInt32 len);

	void 				WriteChar(int ch);
	void 				WriteLine(const std::string & line);
	void 				WriteLine(const char * s);
	void 				Flush();

private:
	
	DlInt32 Send(const void * buffer, DlInt32 len);
	DlInt32 Recieve(void * buffer, DlInt32 len);

	int		fillBuffer();
	
	char		buffer[kBufferSize];
	char*		readPtr;
	char*		readEndPtr;
	char*		writePtr;
	
	std::string	theHost;
	EndpointRef	ep;

	TCall		sndCall;
	InetAddress	hostAddr;
	OTNotifyUPP	notifyProc;
	bool		bound;	
	
	SockContext	theContext;
};

//---------------------------------- Inlines -----------------------------------

inline
const std::string&	
DlSocket::GetHostName() const 
{
	return theHost;
}

inline
int
DlSocket::GetChar() 
{
	return readPtr == readEndPtr ? fillBuffer() : *readPtr++;
}

inline
void
DlSocket::UngetChar()
{
	readPtr--;
}

inline
void
DlSocket::WriteChar(int ch)
{ 
	if (writePtr - buffer == kBufferSize)
		Flush();
	*writePtr++ = ch;
}

inline
void
DlSocket::WriteLine(const std::string & s)
{
	WriteLine(s.c_str());
}

#endif //_H_DlSocket

//	eof
