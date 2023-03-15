// Machine generated IDispatch wrapper class(es) created with Add Class from Typelib Wizard

#import "C:\\Program Files (x86)\\Prior Scientific\\Prior Software\\Prior.dll" no_namespace
// CFilter wrapper class

class CFilter : public COleDispatchDriver
{
public:
	CFilter(){} // Calls COleDispatchDriver default constructor
	CFilter(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CFilter(const CFilter& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// Attributes
public:

	// Operations
public:


	// IFilter methods
public:
	double get_Curve(long Wheel)
	{
		double result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, parms, Wheel);
		return result;
	}
	void put_Curve(long Wheel, double newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_R8 ;
		InvokeHelper(0x1, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Wheel, newValue);
	}
	double get_MaxAcceleration(long Wheel)
	{
		double result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, parms, Wheel);
		return result;
	}
	void put_MaxAcceleration(long Wheel, double newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_R8 ;
		InvokeHelper(0x2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Wheel, newValue);
	}
	double get_MaxSpeed(long Wheel)
	{
		double result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, parms, Wheel);
		return result;
	}
	void put_MaxSpeed(long Wheel, double newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_R8 ;
		InvokeHelper(0x3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Wheel, newValue);
	}
	double get_RunningCurrent(long Wheel)
	{
		double result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, parms, Wheel);
		return result;
	}
	void put_RunningCurrent(long Wheel, double newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_R8 ;
		InvokeHelper(0x4, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Wheel, newValue);
	}
	double get_StandbyCurrent(long Wheel)
	{
		double result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, parms, Wheel);
		return result;
	}
	void put_StandbyCurrent(long Wheel, double newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_R8 ;
		InvokeHelper(0x5, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Wheel, newValue);
	}
	short get_Position(long Wheel)
	{
		short result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, parms, Wheel);
		return result;
	}
	void put_Position(long Wheel, short newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I2 ;
		InvokeHelper(0x6, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Wheel, newValue);
	}
	short get_PositionsPerWheel(long Wheel)
	{
		short result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, parms, Wheel);
		return result;
	}
	void put_PositionsPerWheel(long Wheel, short newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I2 ;
		InvokeHelper(0x7, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Wheel, newValue);
	}
	long MoveToNext(long Wheel)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x8, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Wheel);
		return result;
	}
	long MoveToPrevious(long Wheel)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x9, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Wheel);
		return result;
	}
	long MoveToPosition(long Wheel, long Filter)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0xa, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Wheel, Filter);
		return result;
	}
	long Home(long Wheel)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xb, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Wheel);
		return result;
	}
	long SetYaxisAsFilter(long FilterType, double FilterPulsesPerRev, short NumberFiltersPerWheel, double PositionOneOffset)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_R8 VTS_I2 VTS_R8 ;
		InvokeHelper(0xc, DISPATCH_METHOD, VT_I4, (void*)&result, parms, FilterType, FilterPulsesPerRev, NumberFiltersPerWheel, PositionOneOffset);
		return result;
	}
	long get_HomeOnStart(long Wheel)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xd, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, Wheel);
		return result;
	}
	void put_HomeOnStart(long Wheel, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0xd, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Wheel, newValue);
	}
	long MoveAllToPosition(long fp1, long fp2, long fp3)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0xe, DISPATCH_METHOD, VT_I4, (void*)&result, parms, fp1, fp2, fp3);
		return result;
	}
	long get_CloseShutterDuringMove()
	{
		long result;
		InvokeHelper(0xf, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_CloseShutterDuringMove(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xf, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_FilterTag(long Wheel, long Filter)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x10, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, parms, Wheel, Filter);
		return result;
	}
	void put_FilterTag(long Wheel, long Filter, LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_BSTR ;
		InvokeHelper(0x10, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Wheel, Filter, newValue);
	}
	long get_IsFitted(long Filter)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x11, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, Filter);
		return result;
	}
	CString get_Name(long Wheel)
	{
		CString result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x12, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, parms, Wheel);
		return result;
	}

	// IFilter properties
public:

};
