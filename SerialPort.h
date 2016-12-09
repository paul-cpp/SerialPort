#pragma once

#ifndef _SERIAL_PORT_H_
#define _SERIAL_PORT_H_

#include <Windows.h>
#include <iostream>
#include <string>
#include <stdint.h>

#include "SerialPortException.h"

using namespace std;


#define ERR_MESSAGE(str) cerr<<"ERROR in finction "<<__FUNCTION__<<"\t line "<<__LINE__<<"\n file \
						 " << __FILE__ << endl << str << endl;

#define DEBUG_MESSAGE(str) cout<<"[DEBUG]:\t"<<str<<endl;

static enum BaudRate : DWORD
{
	BaudRate_110 = 110,
	BaudRate_300 = 300,
	BaudRate_600 = 600,
	BaudRate_1200 = 1200,
	BaudRate_2400 = 2400,
	BaudRate_4800 = 4800,
	BaudRate_9600 = 9600,
	BaudRate_14400 = 14400,
	BaudRate_19200 = 19200,
	BaudRate_38400 = 38400,
	BaudRate_56000 = 56000,
	BaudRate_57600 = 57600,
	BaudRate_115200 = 115200,
	BaudRate_128000 = 128000,
	BaudRate_256000 = 256000
};

static enum Direction  {
	Input = 1,
	Output = 2,
	AllDirections = Input | Output
};

static enum DataBits
{
	DataBits_5 = 5,
	DataBits_6 = 6,
	DataBits_7 = 7,
	DataBits_8 = 8,
};

static enum Parity
{
	// from WinBase.h
	ParityNone = PARITY_NONE,
	ParityOdd = PARITY_ODD,
	ParityEven = PARITY_EVEN,
	ParityMark = PARITY_MARK,
	ParitySpace = PARITY_SPACE
};

static enum StopBits
{
	OneStopBit = ONESTOPBIT,
	OneAndHalfStopBits = ONE5STOPBITS,
	TwoStopBits = TWOSTOPBITS,
};

static enum FlowControl
{
	NoflowControl,
	HardwareControl,
	SoftwareControl,
};

static enum PinoutSignal {
	NoSignal = 0x00,							//No signal
	TransmittedDataSignal = 0x01,				//TxD
	ReceivedDataSignal = 0x02,					//RxD
	DataTerminalReadySignal = 0x04,				//DTR
	DataCarrierDetectSignal = 0x08,				//DCD
	DataSetReadySignal = 0x10,					//DSR
	RingIndicatorSignal = 0x20,					//RNG
	RequestToSendSignal = 0x40,					//RTS
	ClearToSendSignal = 0x80,					//CTS
	SecondaryTransmittedDataSignal = 0x100,		//Secondary TxD
	SecondaryReceivedDataSignal = 0x200			//Secondary RxD
};

class SerialPort
{
protected:
	HANDLE m_Handle;

private:
	COMSTAT mst_comStat;
	COMMCONFIG mst_commConfig;
	DCB mst_dcb;
	COMMTIMEOUTS mst_commTimeouts;

	OVERLAPPED mst_overlappedRead;
	OVERLAPPED mst_overlappedWrite;

	string m_portName;
	DWORD m_baudRate;
	WORD m_dataBits;
	WORD m_parity;
	WORD m_stopBit;

	BOOLEAN m_isOverlapped;

public:
	SerialPort();
	~SerialPort();
	HANDLE getHandle() { return m_Handle; }
	//void setHandle(HANDLE handle) { m_Handle = handle; } /*@warning FOR DEBUG ONLY!*/
	void resetInitParams();
	void checkHandleValid();
	void open(std::string portName, DWORD baudrate, WORD dataBits, WORD parity, WORD stopBit, bool asyncMode);
	void open(WORD portNumber, DWORD baudrate, WORD dataBits, WORD parity, WORD stopBit, bool asyncMode);
	BOOL writeOneChar(UCHAR oneChar, WORD maxWaitTime_ms);
	BOOL sendData(UCHAR* data, UINT length);


	bool isOpen();
	void close();

};

#endif	//_SERIAL_PORT_H_

