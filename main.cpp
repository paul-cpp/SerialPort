#include <Windows.h>
#include <iostream>
#include <string>
#include <time.h>

#include "SerialPort.h"
#include "SerialPortEnumerator.h"
#include "SerialPortException.h"
#include "CRC.h"

using namespace std;

#define TEST 1

const short MSGLEN = 20;

void fillSendData(unsigned char* data, int len)
{
	memset(data, 0, len);
	data[0] = 54;
	data[1] = 1;
	data[2] = 15;

	unsigned short crc = Crc16(data, 18);
	memcpy(data, &crc, sizeof(crc));
}


#if (TEST)
//test
int main()
{
	unsigned char msg[MSGLEN];
	fillSendData(msg, MSGLEN);

	try
	{
		SerialPort port;
		port.open(3, BaudRate::BaudRate_115200, DataBits::DataBits_8, Parity::ParityNone, StopBits::OneStopBit, false);
		port.sendData(msg, 20);
	}
	catch (Exception &e){
		e.what();
	}
	//sdfsd


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
