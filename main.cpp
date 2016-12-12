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
	data[0] = 5;
	data[1] = 1;
	data[2] = 15;

	unsigned short crc = Crc16(data, 18);
	memcpy(data + 18, &crc, sizeof(crc));
}

void printArray(const unsigned char* data, int len)
{
	for (int i = 0; i < len; i++)
		cout <<(int) data[i]<<" ";
	cout << endl;
}

#if (TEST)
//test
int main()
{
	setlocale(LC_ALL, "rus");

	clock_t start, end;

	unsigned char msg[MSGLEN];
	fillSendData(msg, MSGLEN);
	printArray(msg, MSGLEN);

	unsigned char buf[MSGLEN];
	memset(buf, 0, MSGLEN);

	int rn = 0;
	try {
		SerialPort port;
		port.open(4, BaudRate::BaudRate_115200, DataBits::DataBits_8, Parity::ParityNone, StopBits::OneStopBit, true);
		while (true) {

			port.sendData(msg, MSGLEN);
			start = clock();
			
			memset(buf, 0, MSGLEN);
			int readResult = port.readData(buf, MSGLEN, 0);

			if (readResult > 0) {
			cout << "time\t" <<  (((double) clock() - start) / (double)CLOCKS_PER_SEC)<< endl;
				printArray(buf, readResult);
			}
		}
	}
	catch (Exception &e) {
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