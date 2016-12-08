#pragma once

#ifndef _SERIAL_PORT_ENUMERATOR_H_
#define _SERIAL_PORT_ENUMERATOR_H_

#include <iostream>
#include <windows.h>
#include <TCHAR.H> 
#include <vector>
#include <atlstr.h>

using namespace std;


static void getPortsList(std::string  sList[], int maxLen) //added function to find the present serial 
{
	TCHAR lpTargetPath[5000]; // buffer to store the path of the COMPORTS
	DWORD test;
	bool gotPort = 0; // in case the port is not found

	int j = 0;
	for (int i = 0; i < 255; i++) // checking ports from COM0 to COM255
	{
		CString str;
		str.Format(_T("%d"), i);
		CString ComName = CString("COM") + CString(str); // converting to COM0, COM1, COM2

		test = QueryDosDevice(ComName, (LPSTR)lpTargetPath, 5000);

		// Test the return value and error if any
		if (test != 0) //QueryDosDevice returns zero if it didn't find an object
		{
			sList[j] = ComName;
			j++;
			gotPort = 1;

			if (j == maxLen)
				return;
		}

		if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			lpTargetPath[10000]; // in case the buffer got filled, increase size of the buffer.
			continue;
		}

	}

	if (!gotPort) // if not port
		cout << "No Active Ports Found" << endl; // to display error message incase no ports found

}


static void getPortsList(vector<string> & vList) //added function to find the present serial 
{
	TCHAR lpTargetPath[5000]; // buffer to store the path of the COMPORTS
	DWORD test;
	bool gotPort = 0; // in case the port is not found

	for (int i = 0; i < 255; i++) // checking ports from COM0 to COM255
	{
		CString str;
		str.Format(_T("%d"), i);
		CString ComName = CString("COM") + CString(str); // converting to COM0, COM1, COM2

		test = QueryDosDevice(ComName, (LPSTR)lpTargetPath, 5000);

		// Test the return value and error if any
		if (test != 0) //QueryDosDevice returns zero if it didn't find an object
		{
			vList.push_back(std::string(ComName));
			gotPort = 1;
		}

		if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			lpTargetPath[10000]; // in case the buffer got filled, increase size of the buffer.
			continue;
		}

	}

	if (!gotPort) // if not port
		cout << "No Active Ports Found" << endl; // to display error message incase no ports found

}

#endif //_SERIAL_PORT_ENUMERATOR_H_
