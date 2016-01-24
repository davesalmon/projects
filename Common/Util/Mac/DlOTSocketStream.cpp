//+
//	DlSocket.cpp
//
//	Copyright © 2001 David C. Salmon. All Rights Reserved
//
//	Contains simple socket class for mac.
//-

//---------------------------------- Includes ----------------------------------

#include "DlPlatform.h"
#include "DlSocket.h"
#include "DlSockException.h"
#include <Threads.h>
#include <LListener.h>

//---------------------------------- Declares ----------------------------------

static OSStatus handleError(EndpointRef ep, OTResult err);
static pascal void YieldingNotifier(void* ctx, OTEventCode code, OTResult res, 
					void* cookie);

//	class to initialize and tear down open transport
class OTInit {
public:

	OTInit();
	~OTInit();

	OSStatus	GetStatus() const { return theStatus; }
	InetSvcRef	GetSvcRef() const { return theRef; }

private:
	
	OSStatus	theStatus;
	InetSvcRef	theRef;
};

static OTInit	sInit;

//---------------------------------- Methods -----------------------------------

//------------------------------------------------------------------------------
//	OTInit::OTInit
//
//		initialize open transport. 
//
//------------------------------------------------------------------------------
OTInit::OTInit()
	: theStatus(0)
//	, theRef(0)
{
	theStatus = InitOpenTransport();
	if (!theStatus)
		// open services
		theRef = OTOpenInternetServices (kDefaultInternetServicesPath, 0, &theStatus);
}

//------------------------------------------------------------------------------
//	OTInit::~OTInit
//
//		close open transport. 
//
//------------------------------------------------------------------------------
OTInit::~OTInit()
{
	if (theRef)
		OTCloseProvider(theRef);
	CloseOpenTransport();
}

//------------------------------------------------------------------------------
//	DlSocket::DlSocket
//
//		build socket connection to host. 
//
//------------------------------------------------------------------------------
DlSocket::DlSocket(const char * host, LListener * l)
	: readPtr(buffer)
	, readEndPtr(buffer)
	, writePtr(buffer)
	, theHost(host)
	, ep(kOTInvalidEndpointRef)
	, notifyProc(0)
	, bound(false)
	
{
	notifyProc = NewOTNotifyUPP(YieldingNotifier);

	theContext.host = host;
	theContext.l = l;

	OSStatus theErr = sInit.GetStatus();
	if (!theErr) {
		ep = OTOpenEndpoint(OTCreateConfiguration(kTCPName), 0, NULL, &theErr);
		if (!theErr)
			theErr = OTSetSynchronous(ep);
		if (!theErr)
			theErr = OTSetBlocking(ep);
		if (!theErr)
			theErr = OTInstallNotifier(ep, notifyProc, &theContext);
		if (!theErr)
			theErr = OTUseSyncIdleEvents(ep, true);
		if (!theErr)
			theErr = OTBind(ep, NULL, NULL);
	
		if (!theErr) {
			bound = true;

			//	convert host name to ip address
			InetHostInfo	info;
			theErr = OTInetStringToAddress (sInit.GetSvcRef(),
											const_cast<char *>(host),
											&info);
			if (!theErr) {
				//	make the connection
				OTInitInetAddress(&hostAddr, 80, info.addrs[0]);
				
				OTMemzero(&sndCall, sizeof(sndCall));
				
				sndCall.addr.buf = (UInt8*)&hostAddr;
				sndCall.addr.len = sizeof(InetAddress);
				sndCall.addr.maxlen = sizeof(InetAddress);
			
				theErr = OTConnect(ep, &sndCall, NULL);
			}
		}
	}

	if (theErr)
		throw DlSockException(host, theErr, DlSockException::CantConnect);
}

//------------------------------------------------------------------------------
//	DlSocket::~DlSocket
//
//		disconnect. 
//
//------------------------------------------------------------------------------
DlSocket::~DlSocket()
{
	if (bound)
		OTUnbind(ep);

	if (ep != kOTInvalidEndpointRef)
		OTCloseProvider(ep);

	if (notifyProc)
		DisposeOTNotifyUPP(notifyProc);

}

//------------------------------------------------------------------------------
//	DlSocket::WriteLine
//
//		write a line of text to the buffer. 
//
//------------------------------------------------------------------------------
void
DlSocket::WriteLine(const char * s)
{
	while (*s)
		WriteChar(*s++);
	WriteChar('\r');
	WriteChar('\n');
}

//------------------------------------------------------------------------------
//	DlSocket::ReadLine
//
//		read a line of text from the socket. Return true if eol found 
//
//------------------------------------------------------------------------------
bool
DlSocket::ReadLine(std::string & line)
{
	line.clear();
	while(true)
	{
		int c = GetChar();
		if (c <= 0)
			return false;
		if (c == '\r')
		{
			c = GetChar();
			if (c <= 0 || c == '\n')
				return true;
			else
				UngetChar();
		} else if (c == '\n') {
			return true;
		}
		
		line += c;
	}
}

//------------------------------------------------------------------------------
//	DlBufferedFileStream::DlBufferedFileStream
//
//		create the stream. 
//
//------------------------------------------------------------------------------
DlInt32
DlSocket::ReadBuffer(void * buf, DlInt32 len)
{
	DlInt32 inBuffer = readEndPtr - readPtr;
	
	if (inBuffer > len)
		inBuffer = len;
	
	std::memcpy(buf, readPtr, inBuffer);
	readPtr = readEndPtr;
	
	if (inBuffer < len)
	{
		DlInt32 rem = Recieve((char *)buf + inBuffer, len - inBuffer);
		return rem + inBuffer;
	}
	return inBuffer;
}

//------------------------------------------------------------------------------
//	DlSocket::fillBuffer
//
//		fill the buffer. 
//
//------------------------------------------------------------------------------
int
DlSocket::fillBuffer()
{
	DlInt32 len = Recieve(buffer, kBufferSize);
	if (len == 0) {
		return -1;
	} else {
		readPtr = buffer;
		readEndPtr = readPtr + len;
	}
	return *readPtr++;
}

//------------------------------------------------------------------------------
//	DlSocket::Flush
//
//		flush the buffer. 
//
//------------------------------------------------------------------------------
void
DlSocket::Flush()
{
	DlInt32 len = writePtr - buffer;
	if (len > 0) {
		Send(buffer, len);
		writePtr = buffer;
	}
}

//------------------------------------------------------------------------------
//	DlSocket::Send
//
//		send len bytes to the socket. 
//
//------------------------------------------------------------------------------
DlInt32
DlSocket::Send(const void * buffer, DlInt32 len)
{
	OTResult bytesSent = OTSnd(ep, const_cast<void *>(buffer), len, 0);
	if (bytesSent < 0)
		throw handleError(ep, bytesSent);

	return bytesSent;
}

//------------------------------------------------------------------------------
//	DlSocket::Recieve
//
//		Read up to len bytes from the socket. 
//
//------------------------------------------------------------------------------
DlInt32
DlSocket::Recieve(void * buffer, DlInt32 len)
{
	OTFlags tmp = 0;
	OTResult bytesGot = OTRcv(ep, buffer, len, &tmp);
	if (bytesGot < 0)
		bytesGot = handleError(ep, bytesGot);

	return bytesGot < 0 ? 0 : bytesGot;
}

//------------------------------------------------------------------------------
//	handleError
//
//		handle error condition. 
//
//------------------------------------------------------------------------------
static OSStatus 
handleError(EndpointRef ep, OTResult err)
{
	if (err == kOTLookErr)
	{
		OSStatus res = OTLook(ep);
		if (res == T_DISCONNECT) {
			err = OTRcvDisconnect(ep, NULL);
		} else if (res == T_ORDREL) {
			err = OTRcvOrderlyDisconnect(ep);
			if (!err)
				err = OTSndOrderlyDisconnect(ep);
		}
	}
	return err;
}

//------------------------------------------------------------------------------
//	YieldingNotifier
//
//		send/recieve is idling 
//
//------------------------------------------------------------------------------
static pascal void 
YieldingNotifier(void* ctx, OTEventCode code, OTResult res, void* cookie)
{
	res; cookie;
	if (code == kOTSyncIdleEvent) {
		SockContext* sockCtx = (SockContext*)ctx;
		if (sockCtx->l && sockCtx->l->IsListening())
			sockCtx->l->ListenToMessage(DlMessageIdle, 0);
			
		YieldToAnyThread();
	}
}

//	eof
