// Machine generated IDispatch wrapper class(es) created with Add Class from Typelib Wizard


// CScan wrapper class

class CScan : public COleDispatchDriver
{
public:
	CScan(){} // Calls COleDispatchDriver default constructor
	CScan(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CScan(const CScan& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// Attributes
public:

	// Operations
public:


	// IScan methods
public:
	long Connect(long * Port)
	{
		long result;
		static BYTE parms[] = VTS_PI4 ;
		InvokeHelper(0x1, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Port);
		return result;
	}
	long IsMoving(long * Status)
	{
		long result;
		static BYTE parms[] = VTS_PI4 ;
		InvokeHelper(0x2, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Status);
		return result;
	}
	long MoveXYZtoAbsolute(double XPos, double YPos, double ZPos)
	{
		long result;
		static BYTE parms[] = VTS_R8 VTS_R8 VTS_R8 ;
		InvokeHelper(0x3, DISPATCH_METHOD, VT_I4, (void*)&result, parms, XPos, YPos, ZPos);
		return result;
	}
	long MoveXYZRelative(double XPos, double YPos, double ZPos)
	{
		long result;
		static BYTE parms[] = VTS_R8 VTS_R8 VTS_R8 ;
		InvokeHelper(0x4, DISPATCH_METHOD, VT_I4, (void*)&result, parms, XPos, YPos, ZPos);
		return result;
	}
	long KillAllMotion()
	{
		long result;
		InvokeHelper(0x5, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long InterruptAllMotion()
	{
		long result;
		InvokeHelper(0x6, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetPowerSupplyVoltage(long * Voltage)
	{
		long result;
		static BYTE parms[] = VTS_PI4 ;
		InvokeHelper(0x7, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Voltage);
		return result;
	}
	long GetBoardTemperature(float * Temperature)
	{
		long result;
		static BYTE parms[] = VTS_PR4 ;
		InvokeHelper(0x8, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Temperature);
		return result;
	}
	long OpenShutter(long Shutter, long OpenNotClose)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x9, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Shutter, OpenNotClose);
		return result;
	}
	float get_Version()
	{
		float result;
		InvokeHelper(0xa, DISPATCH_PROPERTYGET, VT_R4, (void*)&result, NULL);
		return result;
	}
	short get_Revision()
	{
		short result;
		InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
		return result;
	}
	short get_IsAtLimits()
	{
		short result;
		InvokeHelper(0xc, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
		return result;
	}
	long get_IsShutterOpen(long Shutter)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xd, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, Shutter);
		return result;
	}
	void put_IsShutterOpen(long Shutter, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0xd, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Shutter, newValue);
	}
	long get_SerialNumber()
	{
		long result;
		InvokeHelper(0xe, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetFourthParams(long * type, BSTR * Description)
	{
		long result;
		static BYTE parms[] = VTS_PI4 VTS_PBSTR ;
		InvokeHelper(0xf, DISPATCH_METHOD, VT_I4, (void*)&result, parms, type, Description);
		return result;
	}
	long GetFocusParams(long * type, BSTR * Description)
	{
		long result;
		static BYTE parms[] = VTS_PI4 VTS_PBSTR ;
		InvokeHelper(0x10, DISPATCH_METHOD, VT_I4, (void*)&result, parms, type, Description);
		return result;
	}
	long QuerySystem(BSTR * Controller, BSTR * Stage, BSTR * Focus, BSTR * Fourth, BSTR * Filter1, BSTR * Filter2)
	{
		long result;
		static BYTE parms[] = VTS_PBSTR VTS_PBSTR VTS_PBSTR VTS_PBSTR VTS_PBSTR VTS_PBSTR ;
		InvokeHelper(0x11, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Controller, Stage, Focus, Fourth, Filter1, Filter2);
		return result;
	}
	long RestoreDefaults()
	{
		long result;
		InvokeHelper(0x12, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetShuttersPresent(long * Shutter_1, long * Shutter_2, long * Shutter_3)
	{
		long result;
		static BYTE parms[] = VTS_PI4 VTS_PI4 VTS_PI4 ;
		InvokeHelper(0x13, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Shutter_1, Shutter_2, Shutter_3);
		return result;
	}
	long DisConnect()
	{
		long result;
		InvokeHelper(0x15, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long QueryEeprom(BSTR * content)
	{
		long result;
		static BYTE parms[] = VTS_PBSTR ;
		InvokeHelper(0x16, DISPATCH_METHOD, VT_I4, (void*)&result, parms, content);
		return result;
	}
	long SyncAndWait(long Delay)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x17, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Delay);
		return result;
	}
	long get_LumenLightLevel()
	{
		long result;
		InvokeHelper(0x18, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_LumenLightLevel(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x18, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long LumenLightReset()
	{
		long result;
		InvokeHelper(0x19, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_LumenPower()
	{
		long result;
		InvokeHelper(0x1a, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_LumenPower(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x1a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ControllerFirmwareVersion()
	{
		long result;
		InvokeHelper(0x1b, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	long JoystickButtonFunction(LPCTSTR str)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x1c, DISPATCH_METHOD, VT_I4, (void*)&result, parms, str);
		return result;
	}
	long get_IsConnected()
	{
		long result;
		InvokeHelper(0x1d, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	CString get_ISError()
	{
		CString result;
		InvokeHelper(0x1e, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	long get_IsShutterFitted(long Shutter)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x1f, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, Shutter);
		return result;
	}
	long get_UserFlag()
	{
		long result;
		InvokeHelper(0x20, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_UserFlag(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x20, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void LockCommPort()
	{
		InvokeHelper(0x21, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void UnlockCommPort()
	{
		InvokeHelper(0x22, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void TxCommand(LPCTSTR cmd)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x23, DISPATCH_METHOD, VT_EMPTY, NULL, parms, cmd);
	}
	void RxResponse(BSTR * response)
	{
		static BYTE parms[] = VTS_PBSTR ;
		InvokeHelper(0x24, DISPATCH_METHOD, VT_EMPTY, NULL, parms, response);
	}
	long get_IjoyFitted()
	{
		long result;
		InvokeHelper(0x25, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	CString get_ControllerVariant()
	{
		CString result;
		InvokeHelper(0x26, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}

	// IScan properties
public:

};
