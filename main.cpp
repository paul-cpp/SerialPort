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

const short MSGLEN = 44;
unsigned char msg[MSGLEN];
unsigned char buf[MSGLEN];

void fillSendData(unsigned char* data, int len)
{
	memset(data, 0, len);
	data[0] = 5;
	data[1] = 1;
	data[2] = 15;

	unsigned short crc = Crc16(data, 18);
	memcpy(data + 18, &crc, sizeof(crc));
}

#if (TEST==1)
#define main_virtual() main()
int main_virtual()
{
	setlocale(LC_ALL, "rus");
	SerialPort port;

	clock_t start, end;

	fillSendData(msg, MSGLEN);

	memset(buf, 0, MSGLEN);
	
	int rn = 0;
	try {
		port.open(10, BaudRate::BaudRate_9600, DataBits::DataBits_8, Parity::ParityNone, StopBits::OneStopBit, true);
		port.purge();
		while (true) {

			//port.writeData(msg, MSGLEN,300);
			//cout << "Отправлено:\t";
			//for (int i = 0; i<MSGLEN; i++){
			//	cout <<(int) msg[i] << " ";
			//}
			//cout << endl;
			//Sleep(400);

			memset(buf, 0, MSGLEN);
			int readResult = port.read(buf, MSGLEN, 1000);

			cout << readResult << "\t";
			for (int i = 0; i < MSGLEN; i++)
				cout << buf[i];
			cout << endl;
			cout << "-----------------------------" << endl << endl;
		}
	}
	catch (Exception &e) {
		e.what_msgBox();
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
