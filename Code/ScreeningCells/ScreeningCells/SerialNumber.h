#pragma once
class SerialNumber
{
public:
	SerialNumber();
	~SerialNumber();
protected:
	void getcpuid(unsigned int CPUInfo[4], unsigned int InfoType);
	void getcpuidex(unsigned int CPUInfo[4], unsigned int InfoType, unsigned int ECXValue);
	void get_cpuId(char *pCpuId);

	BOOL GetBaseBoardByCmd(char *lpszBaseBoard, int len/*=128*/);
	bool GetAdapterInfo(int adapterNum, std::string& macOUT);
	bool GetMacByNetBIOS(std::string& macOUT);

	void read_reg_sz(char buf[256]);
	void write_reg_sz(const char* m_name_set);
	void delete_value();
	void delete_key();

public:
	bool CheckPcID();
};

