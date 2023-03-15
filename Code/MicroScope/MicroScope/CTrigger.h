// Machine generated IDispatch wrapper class(es) created with Add Class from Typelib Wizard

#import "C:\\Program Files (x86)\\Prior Scientific\\Prior Software\\Prior.dll" no_namespace
// CTrigger wrapper class

class CTrigger : public COleDispatchDriver
{
public:
	CTrigger(){} // Calls COleDispatchDriver default constructor
	CTrigger(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CTrigger(const CTrigger& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// Attributes
public:

	// Operations
public:


	// ITrigger methods
public:
	long get_encoderCountsPerMicron(long axis)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, axis);
		return result;
	}
	long ArmTrigger(long startPosition, long distanceBetweenTriggers, long axis, long numberOfTriggers, long polarity, long pulseDuration)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x2, DISPATCH_METHOD, VT_I4, (void*)&result, parms, startPosition, distanceBetweenTriggers, axis, numberOfTriggers, polarity, pulseDuration);
		return result;
	}
	long get_IsFitted()
	{
		long result;
		InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}

	// ITrigger properties
public:

};
