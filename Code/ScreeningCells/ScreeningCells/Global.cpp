#include "StdAfx.h"
#include "Global.h"
#include "vendor_code.h"

void displayCloseMessage()
{
	cout << endl << "press ENTER to close the sample" << endl;

	fflush(stdin);
	//while (getchar() == EOF);
}

void displayMemory(unsigned char *mdata, unsigned int mdataLen)
{
	//The number of bytes to be shown in each line
	const unsigned int lineLen = 16;

	//This is printed before the first char in each line
	const char* margin = "     ";

	unsigned int i/*lines*/, ii/*chars*/;

	for (i = 0; i < mdataLen; i += lineLen)
	{
		ostringstream asciiWriter;
		ostringstream hexWriter;

		for (ii = i; ii < lineLen + i; ii++)
		{
			if (ii < mdataLen)
			{
				//Print the active char to the hex view
				unsigned int val = static_cast<unsigned int>(mdata[ii]);
				if (val < 16)
					hexWriter << '0';
				hexWriter << uppercase << hex << val << ' ';

				//... and to the ascii view
				if (mdata[ii] < 32 || mdata[ii]>127) //Do not print some characters
					asciiWriter << '.';             //because this may create
				else                              //problems on UNIX terminals
					asciiWriter << mdata[ii];
			}
			else
			{
				hexWriter << "   ";
				asciiWriter << " ";
			}

			//Display a separator after each 4th byte
			if (!((ii - i + 1) % 4) && (ii - i + 1 < lineLen))
				hexWriter << "| ";
		}

		//Write the line
		cout << margin << hexWriter.str() << " [" << asciiWriter.str() << "]" << endl;
	}
}

void displayTime(const ChaspTime& haspTime)
{
	cout << "     Sentinel key time: " << haspTime.hour();
	cout << ':' << haspTime.minute() << ':' << haspTime.second() << endl;
	cout << "     Sentinel key date: " << haspTime.day();
	cout << '/' << haspTime.month() << '/' << haspTime.year() << " D/M/Y" << endl;
}


haspStatus gEncryptionDogVertify()
{
	cout << endl;
	cout << "This is a simple demo program for Sentinel LDK licensing functions." << endl;
	cout << "Copyright (C) SafeNet, Inc. All rights reserved." << endl << endl;

	//Prints the error messages for the return values of the functions
	ErrorPrinter errorPrinter;

	//Used to hold the return value of the called functions
	haspStatus status;

	Chasp hasp1(ChaspFeature::fromFeature(FeatureId));
	status = hasp1.login(vendorCode);
	errorPrinter.printError(status);

	if (!HASP_SUCCEEDED(status))
	{
		displayCloseMessage();
		return status;
	}

	//Demonstrates how getSessionInfo can be used to create a scope
	//which can be later used for a login

	//This info template is used to create a scope which limits
	//the login to the current key
	std::string infoTemplate = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
		"<haspformat root=\"haspscope\">"
		"<hasp>"
		"<attribute name=\"id\" />"
		"</hasp>"
		"</haspformat>";

	//The output of the getInfo call
	std::string resultingScope;

	cout << "get a login scope                : ";
	status = hasp1.getSessionInfo(infoTemplate, resultingScope);
	errorPrinter.printError(status);

	if (!HASP_SUCCEEDED(status))
	{
		displayCloseMessage();
		return status;
	}

	if (HASP_SUCCEEDED(status))
		hasp1.logout();

	cout << endl;

	return status;
}

int gEncryptionDogLeftCount()
{
	const char* scope =
		"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
		"<haspscope/>";

	const char* format =
		"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
		"<haspformat root=\"hasp_info\">"
		" <feature>"
		" <attribute name=\"id\" />"
		" <element name=\"license\" />"
		" <hasp>"
		" <attribute name=\"id\" />"
		" <attribute name=\"type\" />"
		" </hasp>"
		" </feature>"
		"</haspformat>"
		"";

	unsigned char vendorCode[] =
		"Qc9YHEC02rnNlyR1lZ21wbivHF+bMFBwV+VR3OcVI/Y4bXLFI9JcWUdudtfzoyO02xRfK407HYHbMDmf"
		"EsHpYr7RZiROQ9+XyIR/h9OMVVTYwtZJjx5KBS/WjanAFEXoh2irLhYumZACXtTYcrnvO9NGt7bYNJee"
		"zt5XPwUf5tVxemM50FEfB6/tR4YwigxBSa7rS9O5K5yZtPA2Qu6TkRyNLH2EQFsMTCgHVUUiFv5pcRX8"
		"tC8XxEtkl2DmKs3ggc9YfLhjvSYLsqsYG1hEE4xRsG96HvsAo7Wrq/J5Z3OYXLE3WRb2nIjAyThGHREr"
		"ErjXyaF3wLaPfBUM8Gn7HAWDkvtX80SDlkAy5xTMOOK/RXKimozYHuFF+2cQvsR6/jKES/Q90yKaalOl"
		"lE4TyUCejoX2Kg64EE6o6JFGB0gxL1uqD4igJks1zvf0jR/Pe1DxEP6vFEGtssE3b9eEGTFr/eqsroDq"
		"2zkxTwEFrBNtDs5s+d08L5XfxAdd25lZP38QA/seiIV2iYXwvX/AsQ3QB27Y+gBiesvnI7VvHzD+8Jvi"
		"ttkL0YN7dDgwJXNYy45jnK4LACLf3e07xuP3dlvOQS8Rkar6o9h5dYIh8yRUbZsoO+d3ATSVSEWBEWqM"
		"4yTKIubLxYt/BuO0jSRt9BhbBp507dJ4fO+AXBInMurBlH+8K6rwynQ4GlcDzyxxd1OjlQlZXrdNN9Ra"
		"wN42fnhIsv25C/+utqUhGuq1nAgZSKRBOAiX8vzGrVHKZOa8D1CjLDrSZE6pS6chCDi16SygGsfFHEUD"
		"Mg6upbHa9DuVZCNuhlpYUps4U5uG5QebcgvFPWK24+6XZlMcgKWhrxJIboe2cwmdNT2R7LoE/oPalEeP"
		"b8PvCr1BTmdRWUdB7qL77R0GIER+PXhJI0KrfZD41Y8DVHQIT0BUt+XxWk6aIz3PwpgZLaEzvfLSn0JG"
		"enfu31035BsA7/AAbY3lkQ==";

	std::string info;
	haspStatus status = Chasp::getInfo(scope, format, vendorCode, info);

	if (!HASP_SUCCEEDED(status))
	{
		//handle error
		int i = -1;
	}

	if (info.empty())
	{
		return -1;
	}

	CMarkup xml;
	CString strInfo(info.c_str());
	xml.Load(strInfo.GetString());
	CMarkupNode Root(xml.GetRoot());
	//º”‘ÿÕº∆¨◊ ‘¥
 	LPCTSTR pstrId = NULL;
// 	LPCTSTR pstrPath = NULL;
 	LPCTSTR pstrClass = NULL;
	LPCTSTR cntMax = NULL;
	LPCTSTR cntUse = NULL;
	int iMax = 0, iUse = 0, left = 0;
	for (CMarkupNode node = Root.GetChild(); node.IsValid(); node = node.GetSibling())
	{
		pstrClass = node.GetName();
		CMarkupNode ChildNode = node.GetChild();
		if (node.HasAttributes())
		{
			int count = node.GetAttributeCount();
			pstrId = node.GetAttributeValue(_T("id"));
			TCHAR buf[12] = { 0 };
			_stprintf(buf, _T("%d"), FeatureId);
			if (_tcsicmp(pstrId, buf) == 0)
			{
				ChildNode = ChildNode.GetChild();
				int iRecord = 0;
				while (ChildNode.IsValid())
				{
					pstrClass = ChildNode.GetName();
					if (_tcsicmp(pstrClass, _T("counter_fix")) == 0)
					{
						cntMax = ChildNode.GetValue();
						iRecord++;
					}
					if (_tcsicmp(pstrClass, _T("counter_var")) == 0)
					{
						cntUse = ChildNode.GetValue();
						iRecord++;
					}
					if (iRecord == 2 && cntMax && cntUse)
					{
						iMax = _ttoi(cntMax);
						iUse = _ttoi(cntUse);
						left = iMax - iUse;
						if (left < 0)
							left = 0;
						return left;
					}

					ChildNode = ChildNode.GetSibling();
				}
			}
		}
	}
	return 0;
}
