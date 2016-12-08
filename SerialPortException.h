#pragma once 

#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <stdexcept>
#include <Windows.h>
#include <string>
#include <iostream>

class Exception : public std::exception
{
public:
	Exception(DWORD code_)
	{
		code = code_;
		errorString = GetLastErrorToStd(code);
	}

	~Exception(){};

	void what()			{ std::cerr << errorString << std::endl; }
	void what_msgBox()	{ MessageBox(NULL, errorString.c_str(), TEXT("ERROR"), MB_OK | MB_ICONERROR); }

	std::string getErrorString() { return errorString; }
	DWORD getErrorCode() { return code; }

private:
	std::string GetLastErrorToStd(DWORD code)
	{
		if (code == 0)
			return std::string();	//no errors

		LPSTR mesBuf = NULL;
		size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&mesBuf, 0, NULL);
		std::string message(mesBuf, size);

		LocalFree(mesBuf);

		return message;
	}

	DWORD code;						//error code
	std::string errorString;		//string message code

};

#endif//_EXCEPTION_H_
