//+
//	DlAppleEvent.h
//
//	Copyright © 1999, David C. Salmon. All Rights Reserved
//
//	Contains the interface for sending apple events to other applications
//
//-

#ifndef _H_DlAppleEvent

//------------------------------ Includes ------------------------------

//#include <AppleEvents.h>
//#include <Processes.h>
//#include <MacErrors.h>

//------------------------------ Defines -------------------------------

const ProcessSerialNumber	kNoPSN = { 0, 0 };
const AppleEvent			kNoEvent = { typeNull, 0 };

//------------------------------ Declares ------------------------------

//	return true if the PSN is NULL
inline bool
operator ! (const ProcessSerialNumber& p1)
{
	return p1.highLongOfPSN == kNoPSN.highLongOfPSN && 
		p1.lowLongOfPSN == kNoPSN.lowLongOfPSN;
}

//	return true if the AppleEvent (AEDesc) is NULL
inline bool
operator ! (const AppleEvent& p1)
{
	return p1.dataHandle == kNoEvent.dataHandle && 
		p1.descriptorType == kNoEvent.descriptorType;
}

//----------------------------------------------------------------------
//	class	DlAppleEvent
//
//		handle sending apple events to other applications
//----------------------------------------------------------------------
class	DlAppleEvent {
public:
			//	constructor
#if ! _LP64__
			DlAppleEvent(OSType	signature, const FSSpec& spec);
#endif
			//	constructor
			explicit DlAppleEvent(const ProcessSerialNumber& psn) :
					itsEvent(kNoEvent), itsReply(kNoEvent),
					itsProcess(psn) 
			{}

			//	destructor
			~DlAppleEvent() {
				dispose();
			}

	//	specify/create the event to send
	void	SpecifyEvent(DescType classID, DescType eventID);

	//	add a parameter descriptor to the event
	void	AddParam(AEKeyword key, DescType inType, 
				const void* inData, SInt32 inSize);

	//	add a keyword specified descriptor to the event
	void	AddKey(AEKeyword key, DescType inType, 
				const void* inData, SInt32 inSize);

	//	add an attribute descriptor to the event
	void	AddAttribute(AEKeyword key, DescType inType, 
				const void* inData, SInt32 inSize);

	//	launch/wake up the application
	void	ActivateApplication();

	//	send the event, if idleProc is not NULL, a reply is waited for
	void	Send(AESendMode mode, AEIdleProcPtr idleProc);

	//	get the reply
	const AppleEvent& 
			GetReply() const {
				return itsReply;
			}

	//	get the serial number
	const ProcessSerialNumber& 
			GetProcess() const {
				return itsProcess;
			}

#if !__LP64__
	//	find the application or launch if not found
	static ProcessSerialNumber
			FindRunningApplication(OSType signature, const FSSpec& spec);
#endif
private:

	//	dispose the event and reply
	void	dispose();

	AppleEvent			itsEvent;
	AppleEvent			itsReply;
	ProcessSerialNumber	itsProcess;
};

#endif	// _H_DlAppleEvent

//	eof
