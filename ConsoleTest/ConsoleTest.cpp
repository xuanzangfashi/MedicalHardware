// ConsoleTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#pragma comment(lib,"MedicalHardware.lib")

__declspec(dllexport) bool InitMedicalHardwareAPI(char* COMName, long baudRate, int bytessize);

__declspec(dllexport) void ReleaseMedicalHardwareAPI();
__declspec(dllexport) int GetNumOfData();
__declspec(dllexport) const char* GetBytesByDataIndex(int index);


int main(int argc, char **argv)
{
	bool re = false;
	
	
	
	
	int i = 0;
	while (true)
	{
		std::cout << "InputCOM_Name" << std::endl;
		char *name = new char;
		std::cin >> name;
		bool re =InitMedicalHardwareAPI(name, 19200l, 5);
		if (!re)
			std::cout << "Failed" << std::endl;
		else
		{
			std::cout << "Succeed" << std::endl;
			//break;
		}
		//printf(GetBytesByDataIndex(0));
		//Sleep(500);
		
		//i++;
		//if(getch() == 'j')
		//	ReleaseMedicalHardwareAPI();
		//if (getch() == 'o')
		//	InitMedicalHardwareAPI("COM3", 19200l, 5);
		//system("cls");
	}
	while (1)
	{
		//printf(GetBytesByDataIndex(0));
	}
	getchar();
}
