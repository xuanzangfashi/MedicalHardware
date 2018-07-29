#include "MedicalHardware.h"
#include "tchar.h"
#define _CRT_SECURE_NO_WARNINGS
MedicalHardwareAPI::MedicalHardwareAPI()
{
	SerialPorts = new std::list<SerialPortData*>();
}

MedicalHardwareAPI::~MedicalHardwareAPI()
{
	delete SerialPorts;
}
MedicalHardwareAPI* MedicalHardwareAPI::instance = NULL;
MedicalHardwareAPI *MedicalHardwareAPI::GetInstance()
{
	if (!instance)
	{
		instance = new MedicalHardwareAPI();
	}
	return instance;
}

SerialPortData::SerialPortData(char *COMName, long baudRate, int bytessize, bool* bIsSucceed)
{
	BreakThread = false;
	bool re = true;
	this->port_name = new char;
	this->bytes_size = bytessize;
	strcpy(this->port_name, COMName);
	this->baud_rate = baudRate;
	this->buffer = new char[255];
	this->OutsideBuffer = new char[255];
	memset(this->buffer, 0, 255);
	memset(this->OutsideBuffer, 0, 255);
	serial_port = CreateFileA((port_name), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	if (serial_port == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "Error opening port\n");
		CloseHandle(serial_port);
		re = false;
	}
	else
	{
		DCB properties;
		GetCommState(serial_port, &properties);
		switch (this->baud_rate)
		{
		case 1200:
			properties.BaudRate = CBR_1200;
			break;
		case 2400:
			properties.BaudRate = CBR_2400;
			break;
		case 4800:
			properties.BaudRate = CBR_4800;
			break;
		case 9600:
			properties.BaudRate = CBR_9600;
			break;
		case 14400:
			properties.BaudRate = CBR_14400;
			break;
		case 19200:
			properties.BaudRate = CBR_19200;
			break;
		case 38400:
			properties.BaudRate = CBR_38400;
			break;
		}
		properties.Parity = NOPARITY;
		properties.ByteSize = 8;
		properties.StopBits = ONESTOPBIT;
		SetCommState(serial_port, &properties);
	}
	if (re)
	{
		this->MutexHandle = CreateMutex(NULL, FALSE, NULL);
		ThreadHandle = CreateThread(0, 0, LPTHREAD_START_ROUTINE(threadFun), LPVOID(this), CREATE_SUSPENDED, &ThreadID);
		ResumeThread(ThreadHandle);
	}
	*bIsSucceed = re;
}

SerialPortData::~SerialPortData()
{
	printf("%s", this->port_name);
	printf(" deleting\r\n     ");
	if (this->buffer != NULL)
		delete[] this->buffer;
	delete[] this->OutsideBuffer;
	delete this->port_name;
	CloseHandle(serial_port);
	printf("deleted\r\n");
}

#pragma optimize("",off)
DWORD WINAPI threadFun(SerialPortData* Data)
{
	//char buffer[200];      /* Buffer to store data */
	char tmpBuffer[1] = { "" };
	WaitForSingleObject(Data->MutexHandle, INFINITE);
	while (!Data->BreakThread)
	{

		memset(tmpBuffer, 0, 1);
		ReadFile(Data->serial_port, (void *)tmpBuffer, (long)1, &Data->bytes_received, NULL);
		if ((*tmpBuffer < 23 || *tmpBuffer > 82) && *tmpBuffer != 10 && *tmpBuffer != 13)
		{
			memset(Data->buffer, 0, 255);
			memset(Data->OutsideBuffer, 0, 255);
			continue;
		}
		if (*tmpBuffer == 'E')
		{
			strcpy(Data->OutsideBuffer, Data->buffer);
			memset(Data->buffer, 0, 255);
			OutputDebugStringA(Data->OutsideBuffer);
			printf("%s\n", Data->OutsideBuffer);
			//delete[] tmpBuffer;
			continue;
		}
		strcat(Data->buffer, tmpBuffer);
		//delete[] tmpBuffer;



	}
	
	ReleaseMutex(Data->MutexHandle);
	return THREAD_EXIT;
}
#pragma optimize("",on)


__declspec(dllexport) bool InitMedicalHardwareAPI(char* COMName, long baudRate, int bytessize)
{
	bool re = false;
	SerialPortData* tmpSerialPort = new SerialPortData(COMName, baudRate, bytessize, &re);
	if (!re)
	{
		delete tmpSerialPort;
	}
	else
	{
		
		MedicalHardwareAPI::GetInstance()->SerialPorts->push_back(tmpSerialPort);
	}
	return re;
}

__declspec(dllexport) void ReleaseMedicalHardwareAPI()
{
	std::list<SerialPortData*>::iterator iter = MedicalHardwareAPI::GetInstance()->SerialPorts->begin();

	for (; iter != MedicalHardwareAPI::GetInstance()->SerialPorts->end(); )
	{
		(*iter)->BreakThread = true;
		SerialPortData* tmp = *iter;
		iter = MedicalHardwareAPI::GetInstance()->SerialPorts->erase(iter);

		tmp->BreakThread = true;
		DWORD ThreadRe = WaitForSingleObject(tmp->MutexHandle, 1000);
		ReleaseMutex(tmp->MutexHandle);
		switch (ThreadRe)
		{
		case WAIT_OBJECT_0:
			printf("Thread Done!");
			break;
		case WAIT_TIMEOUT:
			TerminateThread(tmp->ThreadHandle, -1);
			break;
		case WAIT_FAILED:
			TerminateThread(tmp->ThreadHandle, -1);
			break;
		}
		CloseHandle(tmp->ThreadHandle);
		CloseHandle(tmp->MutexHandle);
		delete tmp;
	}
}

__declspec(dllexport) const char* GetBytesByDataIndex(int index)
{
	std::list<SerialPortData*>::iterator iter = MedicalHardwareAPI::GetInstance()->SerialPorts->begin();
	int t_index = 0;
	for (; iter != MedicalHardwareAPI::GetInstance()->SerialPorts->end(); iter++)
	{
		if (t_index == index)
		{
			//printf("%s\n", (*iter)->OutsideBuffer);
			return (*iter)->OutsideBuffer;
		}
		t_index++;
	}
	return "";
}

//__declspec(dllexport) const char* GetBytesByDataIndex(int index)
//{
//	std::list<SerialPortData*>::iterator iter = MedicalHardwareAPI::GetInstance()->SerialPorts->begin();
//	int t_index = 0;
//	for (; iter != MedicalHardwareAPI::GetInstance()->SerialPorts->end(); iter++)
//	{
//		if (t_index == index)
//			return (*iter)->OutsideBuffer;
//		t_index++;
//	}
//	return "";
//}

__declspec(dllexport) void GetAllData(char** &Out)
{
	std::list<SerialPortData*>::iterator iter = MedicalHardwareAPI::GetInstance()->SerialPorts->begin();
	for (int i = 0; iter != MedicalHardwareAPI::GetInstance()->SerialPorts->end(); iter++, i++)
	{
		strcpy(Out[i], (*iter)->OutsideBuffer);
	}
}

__declspec(dllexport) int GetNumOfData()
{
	return MedicalHardwareAPI::GetInstance()->SerialPorts->size();
}

