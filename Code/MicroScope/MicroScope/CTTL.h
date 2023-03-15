// Machine generated IDispatch wrapper class(es) created with Add Class from Typelib Wizard

#import "C:\\Program Files (x86)\\Prior Scientific\\Prior Software\\Prior.dll" no_namespace
// CTTL wrapper class

class CTTL : public COleDispatchDriver
{
public:
	CTTL(){} // Calls COleDispatchDriver default constructor
	CTTL(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CTTL(const CTTL& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// Attributes
public:

	// Operations
public:


	// ITTL methods
public:
	long get_Input(short Bit)
	{
		long result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, Bit);
		return result;
	}
	long get_Output(short Bit)
	{
		long result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, Bit);
		return result;
	}
	void put_Output(short Bit, long newValue)
	{
		static BYTE parms[] = VTS_I2 VTS_I4 ;
		InvokeHelper(0x2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Bit, newValue);
	}
	long DeleteTriggers(short Bit)
	{
		long result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x4, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Bit);
		return result;
	}
	long RestartTriggers(short Bit)
	{
		long result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x5, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Bit);
		return result;
	}
	long AddAction(short Bit, long cmd, double data1, double data2, double data3)
	{
		long result;
		static BYTE parms[] = VTS_I2 VTS_I4 VTS_R8 VTS_R8 VTS_R8 ;
		InvokeHelper(0x6, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Bit, cmd, data1, data2, data3);
		return result;
	}
	long AddTrigger(short Bit, long HiLo)
	{
		long result;
		static BYTE parms[] = VTS_I2 VTS_I4 ;
		InvokeHelper(0x7, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Bit, HiLo);
		return result;
	}
	long InMotionTTL(short Bit, long HiLo)
	{
		long result;
		static BYTE parms[] = VTS_I2 VTS_I4 ;
		InvokeHelper(0x8, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Bit, HiLo);
		return result;
	}
	long get_Trigger()
	{
		long result;
		InvokeHelper(0x9, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_Trigger(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x9, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}

	// ITTL properties
public:

};
