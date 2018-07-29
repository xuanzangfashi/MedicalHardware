#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#define  HYCOMMONWINAPI_EXPORTS
#define THREAD_EXIT 99
#ifdef HYCOMMONWINAPI_EXPORTS
#define HYCOMMONWINAPI_API __declspec(dllexport)
#else
#define HYCOMMONWINAPI_API __declspec(dllimport)
#endif
class SerialPortData;
DWORD WINAPI threadFun(SerialPortData* Data);
class SerialPortData
{
public:
	SerialPortData(char* COMName, long baudRate,int bytessize, bool* bIsSucceed);
	~SerialPortData();
public:
	HANDLE serial_port;     /* Handle to the serial port */
	long baud_rate = 9600;    /* Baud rate */
	char* port_name;			/* Name of serial port */
	unsigned long file_size;   /* Size of file to receive in bytes */
	unsigned long bytes_received;  /* Bytes received from serial port */
	unsigned long file_name_size;  /* Size of file name in bytes */
	int bytes_size;
	DWORD ThreadID;		 /* Name of file to receive */
	HANDLE ThreadHandle;
	HANDLE MutexHandle;
	char* OutsideBuffer;
	char* buffer;
	bool BreakThread;
public:

private:

};


class MedicalHardwareAPI
{
public:
	MedicalHardwareAPI();
	~MedicalHardwareAPI();
public:
	static MedicalHardwareAPI* GetInstance();
	std::list<SerialPortData*>* SerialPorts;
public:
	
private:
	static MedicalHardwareAPI* instance;
};

__declspec(dllexport) bool InitMedicalHardwareAPI(char* COMName, long baudRate, int bytessize);
__declspec(dllexport) void ReleaseMedicalHardwareAPI();
__declspec(dllexport) const char* GetBytesByDataIndex(int index);
__declspec(dllexport) void GetAllData(char** &Out);
__declspec(dllexport) int GetNumOfData();