//+
//	DlAppleEvent.h
//
//	Copyright © 1999, David C. Salmon. All Rights Reserved
//
//	Contains the interface for sending apple events to other applications
//
//-

//------------------------------ Includes ------------------------------

#include "DlPlatform.h"
#include "DlAppleEvent.h"
#include "DlException.h"

#include <string>

//------------------------------ Defines -------------------------------

#if !__MACH__
using std::strlen;
#endif

//------------------------------ Declares ------------------------------

//----------------------------------------------------------------------
//	class	StAEDesc
//
//		handle construction and destruction of AppleEvent descriptors
//----------------------------------------------------------------------
class	StAEDesc {
public:
	StAEDesc() : itsDesc(kNoEvent) {}
	
	StAEDesc(const ProcessSerialNumber& psn) : itsDesc(kNoEvent) {
		Assign(psn);
	}
	
	StAEDesc(OSType t) : itsDesc(kNoEvent) {
		Assign(t);
	}
	
	StAEDesc(const char* str) : itsDesc(kNoEvent) {
		Assign(str);
	}
	
	StAEDesc(ConstStr255Param str) : itsDesc(kNoEvent) {
		Assign(str);
	}
	
	~StAEDesc() {
		dispose();
	}

	operator	AEDesc* ()				{ return &itsDesc; }
	operator	const AEDesc* () const	{ return &itsDesc; }
	
	operator	AEDesc& () 				{ return itsDesc; }
	operator	const AEDesc& () const	{ return itsDesc; }

	void		Assign(DescType inType, const void* inData, 
						SInt32 inSize) {
					dispose();
					OSErr theErr = ::AECreateDesc(inType, inData, inSize, &itsDesc);
					if (theErr != noErr)
						throw DlException("Failed to create apple event descriptor (%d)", theErr);
				}
	
	void		Assign(const ProcessSerialNumber& psn) {
					Assign(typeProcessSerialNumber, &psn, 
						sizeof(ProcessSerialNumber));
				}

	void		Assign(OSType t) {
					Assign(typeType, &t, sizeof(OSType));
				}

	void		Assign(const char* str) {
					Assign(typeChar, str, strlen(str));
				}

	void		Assign(ConstStr255Param str) {
					Assign(typeChar, str + 1, str[0]);
				}

	bool		IsNULL() const {
					return !!itsDesc;
				}

private:
		
	void	dispose() {
				if (!IsNULL()) {
					::AEDisposeDesc(&itsDesc);
					itsDesc = kNoEvent;
				}
			}

	AEDesc		itsDesc;

};

//	find the application from process manager and get PSN
static bool	findRunningApp(OSType creator, ConstStr255Param procName, 
		ProcessSerialNumber& psn);
		
//	launch the app and get PSN
static bool	launchApp(const FSSpec& appSpec, ProcessSerialNumber& psn);

//------------------------------ Functions -----------------------------

#if !__LP64__
DlAppleEvent::DlAppleEvent(OSType signature, const FSSpec& spec) : 
		itsEvent(kNoEvent), itsReply(kNoEvent),
		itsProcess(kNoPSN) 
{
	itsProcess = FindRunningApplication(signature, spec);
	if (!itsProcess)
		throw DlException("Failed to find application %#s", spec.name);
}
#endif

//----------------------------------------------------------------------
//	DlAppleEvent::SpecifyEvent
//
//		Specify the event to send and create apple event
//
//	classID		->	the class of the event
//	eventID		->	event id
//----------------------------------------------------------------------
void
DlAppleEvent::SpecifyEvent(DescType classID, DescType eventID)
{
	dispose();

	StAEDesc	target(itsProcess);

	OSErr theErr = ::AECreateAppleEvent(
						classID, eventID, target,
						kAutoGenerateReturnID,
						kAnyTransactionID,
						&itsEvent);
	if (theErr != noErr)
		throw DlException("Failed to create apple event (%d)", theErr);
}

//----------------------------------------------------------------------
//	DlAppleEvent::AddParam
//
//		Add direct parameter to the event
//
//	key			->	the keyword of the parameter
//	inType		->	descriptor type
//	inData		->	the data
//	inSize		->	the size
//----------------------------------------------------------------------
void
DlAppleEvent::AddParam(AEKeyword key, DescType inType, 
			const void* inData, SInt32 inSize)
{
	OSErr theErr = ::AEPutParamPtr(&itsEvent, key, inType, inData, inSize);
	if (theErr != noErr)
		throw DlException("Failed to add parameter to apple event (%d)", theErr);

}

//----------------------------------------------------------------------
//	DlAppleEvent::AddKey
//
//		Add keyword descriptor to the event
//
//	key			->	the keyword for this key
//	inType		->	descriptor type
//	inData		->	the data
//	inSize		->	the size
//----------------------------------------------------------------------
void
DlAppleEvent::AddKey(AEKeyword key, DescType inType, 
			const void* inData, SInt32 inSize)
{
	OSErr theErr = ::AEPutKeyPtr(&itsEvent, key, inType, inData, inSize);
	if (theErr != noErr)
		throw DlException("Failed to add key to apple event (%d)", theErr);
}

//----------------------------------------------------------------------
//	DlAppleEvent::AddAttribute
//
//		Add attribute descriptor to the event
//
//	key			->	the keyword for the attribute
//	inType		->	descriptor type
//	inData		->	the data
//	inSize		->	the size
//----------------------------------------------------------------------
void
DlAppleEvent::AddAttribute(AEKeyword key, DescType inType, 
			const void* inData, SInt32 inSize)
{
	OSErr theErr = ::AEPutAttributePtr(&itsEvent, key, inType, inData, inSize);
	if (theErr != noErr)
		throw DlException("Failed to add attribute to apple event (%d)", theErr);
}

//----------------------------------------------------------------------
//	DlAppleEvent::Send
//
//		Send the event. Note that if an idle proc is specified, the
//		mode is set to kAEWaitReply, and we dont return until there
//		is a reply.
//
//	mode		->	any special attributes
//	idleProc	->	idle function
//----------------------------------------------------------------------
void
DlAppleEvent::Send(AESendMode mode, AEIdleProcPtr idleProc)
{
	AEIdleUPP	idleUPP = 0;
	if (idleProc) {
		mode |= kAEWaitReply;
		idleUPP = NewAEIdleUPP(idleProc);
	}
	
	if ((mode & kAEWaitReply) == 0) {
		mode |= kAENoReply;
	}

	OSErr theErr = ::AESend(&itsEvent, &itsReply, mode, 
		kAENormalPriority, kNoTimeOut, idleUPP, NULL);
	
	if (idleUPP) {
		DisposeAEIdleUPP(idleUPP);
	}
	
	if (theErr != noErr)
		throw DlException("Failed to send apple event (%d)", theErr);
}

#if !__LP64__

//----------------------------------------------------------------------
//	FindRunningApplication
//
//		Find or launch the specified application.
//
//	signature		->	the application signature
//	spec			->	the file spec for the app.
//	returns			<-	the process serial number for the application
//----------------------------------------------------------------------
ProcessSerialNumber
DlAppleEvent::FindRunningApplication(OSType signature, const FSSpec& spec)
{
	ProcessSerialNumber	psn = kNoPSN;
	if (!findRunningApp(signature, spec.name, psn)) {
		launchApp(spec, psn);
	}

	return psn;
}

#endif

//----------------------------------------------------------------------
//	dispose
//
//		dispose of our events.
//
//----------------------------------------------------------------------
void
DlAppleEvent::dispose()
{
	if (!!itsEvent) {
		AEDisposeDesc(&itsEvent);
		itsEvent = kNoEvent;
	}
	
	if (!!itsReply) {
		::AEDisposeDesc(&itsReply);
		itsReply = kNoEvent;
	}
}

static bool namesMatch(ConstStringPtr n1, ConstStringPtr n2)
{
	int l1 = n1[0];
	int l2 = n2[0];
	if (l1 != l2)
	return false;
	
	for (int i = 1; i <= l1; i++)
		if (n1[i] != n2[i])
			return false;
	return true;
}

//----------------------------------------------------------------------
//	findRunningApp												(static)
//
//		find the application that is running.
//
//	creator		->	the creator of the process
//	procName	->	name of the process
//	psn			<-	process serial number
//	return		<-	true if PSN is valid
//----------------------------------------------------------------------
static bool
findRunningApp(OSType creator, ConstStr255Param procName, 
		ProcessSerialNumber& psn)
{
	//	specify process start and get the first PSN
	ProcessSerialNumber	myPSN = { 0, 0 };
	OSErr	theErr = ::GetNextProcess(&myPSN);

	//	while GetNextProcess return valid values, check the info.
	while(theErr == noErr) {
		ProcessInfoRec	procInfo;
		Str32			procInfoName;

		//	initialize info rec
		procInfo.processInfoLength = sizeof(ProcessInfoRec);
		procInfo.processName = procInfoName;
#if !__LP64__
		procInfo.processAppSpec = 0;
#else
		procInfo.processAppRef = 0;
#endif
		//	get the information
		theErr  =::GetProcessInformation(&myPSN, &procInfo);

		if (theErr != noErr)
			throw DlException("Failed to get process information (%d)", theErr);
		if (procInfo.processSignature == creator && 
			namesMatch(procName, procInfoName)) {
			psn = myPSN;
			return true;
		}

		theErr = ::GetNextProcess(&myPSN);
		
	}
	return false;
}

#if ! __LP64__

//----------------------------------------------------------------------
//	launchApp													(static)
//
//		launch the application.
//
//	appSpec		->	the file spec for the application
//	psn			<-	process serial number
//	return		<-	true if PSN is valid
//----------------------------------------------------------------------
static bool
launchApp(const FSSpec& appSpec, ProcessSerialNumber& psn)
{
	LaunchParamBlockRec	pb;
	
	pb.launchBlockID = extendedBlock;
	pb.launchEPBLength = extendedBlockLen;
	pb.launchFileFlags = 0;
	pb.launchControlFlags = launchContinue | launchNoFileFlags;
	pb.launchAppSpec = const_cast<FSSpec*>(&appSpec);
	pb.launchAppParameters = NULL;
	
	OSErr anErr = LaunchApplication(&pb);
		
	if (anErr == noErr) {
		psn = pb.launchProcessSN;
	}
	
	return anErr == noErr;
}

#endif

//	eof
