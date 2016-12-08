#include <Windows.h>
#include <iostream>
#include <string>
#include <time.h>

#include "SerialPort.h"
#include "SerialPortEnumerator.h"
#include "SerialPortException.h"

using namespace std;

#define TEST 1

#if (TEST)

//test
int main()
{
	try
	{


		SerialPort port;
		port.open("COM1", BaudRate::BaudRate_115200, DataBits::DataBits_8, Parity::ParityNone, StopBits::OneStopBit, false);
		//port.checkHandleValid();
	}
	catch (Exception &e){
		e.what();
	}


	cin.get();
	return 0;
}

#else

int main()
{


	cin.get();
	return 0;
}

#endif
