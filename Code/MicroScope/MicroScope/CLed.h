// Machine generated IDispatch wrapper class(es) created with Add Class from Typelib Wizard

#import "C:\\Program Files (x86)\\Prior Scientific\\Prior Software\\Prior.dll" no_namespace
// CLed wrapper class

class CLed : public COleDispatchDriver
{
public:
	CLed(){} // Calls COleDispatchDriver default constructor
	CLed(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CLed(const CLed& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// Attributes
public:

	// Operations
public:


	// ILed methods
public:
	long get_MaxPower(long LedId)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, LedId);
		return result;
	}
	long get_Power(long LedId)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, LedId);
		return result;
	}
	void put_Power(long LedId, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, LedId, newValue);
	}
	long get_Lambda(long LedId)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, LedId);
		return result;
	}
	long get_SerialNumber(long LedId)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, LedId);
		return result;
	}
	long get_State(long LedId)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, LedId);
		return result;
	}
	void put_State(long LedId, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x5, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, LedId, newValue);
	}
	long get_IsFitted(long LedId)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, LedId);
		return result;
	}
	long get_Mode(long LedId)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, LedId);
		return result;
	}
	void put_Mode(long LedId, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x7, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, LedId, newValue);
	}
	long get_Compatability()
	{
		long result;
		InvokeHelper(0x8, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_Compatability(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x8, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_PulseTime(long LedId)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x9, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, LedId);
		return result;
	}
	void put_PulseTime(long LedId, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x9, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, LedId, newValue);
	}
	long get_MaxPulseTime(long LedId)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xa, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, LedId);
		return result;
	}
	long get_TriggerType(long LedId)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, LedId);
		return result;
	}
	void put_TriggerType(long LedId, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0xb, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, LedId, newValue);
	}
	long get_Trigger(long LedId)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xc, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, LedId);
		return result;
	}
	void put_Trigger(long LedId, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0xc, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, LedId, newValue);
	}
	CString get_Name(long LedId)
	{
		CString result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xd, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, parms, LedId);
		return result;
	}

	// ILed properties
public:

};
