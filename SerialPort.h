#pragma once

#ifndef _SERIAL_PORT_H_
#define _SERIAL_PORT_H_

#include <Windows.h>
#include <iostream>
#include <string>
#include <stdint.h>

#include "SerialPortException.h"

using namespace std;

#define ERR_MESSAGE(str) cerr<<str<<"\nERROR: "<<__FUNCTION__<<"  line "<<__LINE__ << endl;

#define DEBUG_MESSAGE(str) cout<<"[DEBUG]:\t"<<str<<endl;

/**
 * @enum	BaudRate
 *
 * @brief	�������� �������� �������� ��� ����������������� ����� (� �����). 
 */
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

/**
 * @enum	Direction
 *
 * @brief	����������� �������� ������
 * @warning �� ������������, ��� �������������
 */
static enum Direction  {
	Input = 1,
	Output = 2,
	AllDirections = Input | Output
};

/**
 * @enum	DataBits
 *
 * @brief	���������� ��� ������������ ������
 */

static enum DataBits
{
	DataBits_5 = 5,
	DataBits_6 = 6,
	DataBits_7 = 7,
	DataBits_8 = 8,
};

/**
 * @enum	Parity
 *
 * @brief	��������
 */

static enum Parity
{
	// from WinBase.h
	ParityNone = PARITY_NONE,
	ParityOdd = PARITY_ODD,
	ParityEven = PARITY_EVEN,
	ParityMark = PARITY_MARK,
	ParitySpace = PARITY_SPACE
};

/**
 * @enum	StopBits
 *
 * @brief	����� �������� ���
 */

static enum StopBits
{
	OneStopBit = ONESTOPBIT,
	OneAndHalfStopBits = ONE5STOPBITS,
	TwoStopBits = TWOSTOPBITS,
};

/**
 * @enum	FlowControl
 *
 * @brief	���������� �������
 * @remark  �� ������������
 */

static enum FlowControl
{
	NoflowControl,
	HardwareControl,
	SoftwareControl,
};

/**
 * @enum	PinoutSignal
 *
 * @brief	��������, ������������ ���������� ��������
 */

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

/**
 * @class	SerialPort
 *
 * @brief	A serial port.
 *
 * @author	Paul
 * @date	12/21/2016
 */

class SerialPort
{
protected:
	/** @brief	��������� ����� */
	HANDLE m_Handle;

private:
	/** @brief	��������� ���������� � ��������� ����� */
	COMSTAT mst_comStat;
	/** @brief	��������� ������������ ����� */
	COMMCONFIG mst_commConfig;
	/** @brief	��������� ����� */
	DCB mst_dcb;
	/** @brief	���������, ���������� �������� ��������� �����*/
	COMMTIMEOUTS mst_commTimeouts;

	/** @brief	����������� �������� ������ */
	OVERLAPPED mst_overlappedRead;
	/** @brief	����������� �������� ������ */
	OVERLAPPED mst_overlappedWrite;

	string m_portName;
	DWORD m_baudRate;
	WORD m_dataBits;
	WORD m_parity;
	WORD m_stopBit;

	BOOLEAN m_isOverlapped;

	/**
	* @brief	����� ���������� ���������
	*/
	void resetInitParams();

public:
	SerialPort();	//����������� ��-���������
	~SerialPort();	//���������� ��-���������

	/**
	 * @brief	��������� �������� handle
	 * @return	The handle.
	 */
	HANDLE getHandle() { return m_Handle; }

	/**
	 * @brief	���������� ��������� �� ������� ����� (������ ���� HANDLE)
	 * @param [in,out]	otherPort	HANDLE ������� �����
	 */
	void attach(HANDLE& otherPort);

	/**
	 * @brief	�������� ����� �� ����������
	 * @return	True ���� handle �� ����� INVALID_HANDLE_VALUE, false � ��������� ������
	 */
	BOOL isHandleValid();

	//TODO: ������� bool
	/**
	 * @brief	�������� ����� � ��������� ����������
	 * @param	portName 	��� ����� ������� (��� \\\\.\\)
	 * @param	baudrate 	�������� � �����
	 * @param	dataBits 	���������� ��� ������
	 * @param	parity   	�������� ��������
	 * @param	stopBit  	���������� �������� ���
	 * @param	asyncMode	���� ����������� ��������
	 */
	void open(std::string portName, DWORD baudrate, WORD dataBits, WORD parity, WORD stopBit, bool asyncMode);

	/**
	* @brief	�������� ����� � ��������� ����������
	* @param	portName 	��� ����� ������
	* @param	baudrate 	�������� � �����
	* @param	dataBits 	���������� ��� ������
	* @param	parity   	�������� ��������
	* @param	stopBit  	���������� �������� ���
	* @param	asyncMode	���� ����������� ��������
	*/
	void open(WORD portNumber, DWORD baudrate, WORD dataBits, WORD parity, WORD stopBit, bool asyncMode);

	/**
	 * @brief	������ ������ � ����
	 * @param	data		  	������ ��� ������
	 * @param	length		  	����� ������������ ������
	 * @param	maxWaitTIme_ms	������������ ����� �������� (� �������������) ��� ����������� ��������
	 * @return	���������� ������� ���������� ����.
	 */

	int writeData(const unsigned char* data, UINT length, WORD maxWaitTIme_ms);

	/**
	 * @brief	������ ������ �� ����������������� �����
	 * @param [in,out]	data		  	�������� ����� 
	 * @param 		  	length		  	����� ������
	 * @param 		  	maxWaitTime_ms	������������ ����� �������� (� �������������) ��� ����������� ��������
	 * @return	���������� ������� ����������� ����
	 */
	int readData(unsigned char* data, UINT length, WORD maxWaitTime_ms);

	/**
	 * @fn	void SerialPort::getTimeouts();
	 * @brief	Gets the timeouts.
	 */
	void getTimeouts();
	COMMTIMEOUTS getTimeoutsToStruct();

	/**
	 * @brief	Sets the timeouts. �������� 0 - ������� �� ������������
	 * @param	ReadIntervalTimeout		   	(Optional) The read interval timeout.
	 * @param	ReadTotalTimeout		   	(Optional) The read total timeout.
	 * @param	ReadTotalTimeoutMultiplier 	(Optional) The read total timeout multiplier.
	 * @param	WriteTotalTimeoutConstant  	(Optional) The write total timeout constant.
	 * @param	WriteTotalTimeoutMultiplier	(Optional) The write total timeout multiplier.
	 */
	void setTimeouts(UINT ReadIntervalTimeout = 0, UINT ReadTotalTimeout = 0,
		UINT ReadTotalTimeoutMultiplier = 0, UINT WriteTotalTimeoutConstant = 0,
		UINT WriteTotalTimeoutMultiplier = 0);

	/**
	 * @brief	�������� �� ����������� �����
	 * @return	True if open, false if not.
	 */
	BOOL isOpen();

	/**
	 * @brief	��������� ����
	 * @todo  �������� �� ������������� ��������
	 */
	void close();

	/**
	 * @brief	�������� ��������� ������ � ����
	 * @warning ������ ���������
	 * @param	chr	������������ ������
	 * @todo	�������������� �����������, ���� �������� ������� ����������
	 */
	void transmitChar(unsigned char chr);			//������ ���������

	/**
	 * @brief	������� ������ ����� � ������ ������������ �������� ���� ������ �� ������ �� ����������
	 */
	void flushBuffers();

	/**
	 * @brief	������� �����
	 */
	void purge();

	/**
	 * @brief	������� ������ ������
	 */
	void purgeReadBuffer();

	/**
	 * @brief	������� ������ ������
	 */
	void purgeWriteBuffer();

	/**
	 * @brief	�������� ����������� �������� ������
	 * @param	purgeData	(Optional) ���� True - ������� ����� ������
	 */
	void terminateWrite(BOOL purgeData = true);

	/**
	 * @brief	�������� ����������� �������� ������
	 * @param	purgeData	(Optional)  ���� True - ������� ����� ������
	 */
	void terminateRead(BOOL purgeData = true);

	/**
	 * @fn	void SerialPort::cancelIO();
	 * @brief	�������� ��� ��������� �������� �����/������
	 */
	void cancelIO();

	/**
	 * @brief	������������ �������� ������ ��� �����
	 * 			� ��������� ����� �������� � ��������� �������,
	 * 			���� ClearCommBreak ������� �� ����������.
	 */
	void breakLine();

	/**
	 * @brief	��������������� �������� ������ ��� ����� 
	 * 			� ������������� ����� �������� � ���������� ���������
	 */
	void restoreLine(); 


	///**
	// * @brief	�������� ����� �������� � �������� ������. 
	// * 			��� ������� ������� �� �����, �� ��� �� ������� �������� ReadFile;
	// * @return	������ ������ ������
	// * @warning � ���� ������ ���������� ���� 
	// */
	//DWORD readBufferSize();
	//
	///**
	// * @brief	�������� ����� �������� � ���������� ������. ��� ������� ��� �� �������� � �����
	// * @return	������ ������ ������
	// */
	//DWORD writeBufferSize();

	


	//TODO: ������ **��� � ������ � ����

};

#endif	//_SERIAL_PORT_H_

