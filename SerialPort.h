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
 * @brief	Значения скорости передачи для последовательного порта (в бодах). 
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
 * @brief	Направления передачи данных
 * @warning Не используется, нет необходимости
 */
static enum Direction  {
	Input = 1,
	Output = 2,
	AllDirections = Input | Output
};

/**
 * @enum	DataBits
 *
 * @brief	Количество бит передаваемых данных
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
 * @brief	Четность
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
 * @brief	Число стоповых бит
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
 * @brief	Управление потоком
 * @remark  Не используется
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
 * @brief	Значения, определяющие распиновку сигналов
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
	/** @brief	Описатель порта */
	HANDLE m_Handle;

private:
	/** @brief	Структура информации о состоянии порта */
	COMSTAT mst_comStat;
	/** @brief	Структура конфигурации порта */
	COMMCONFIG mst_commConfig;
	/** @brief	Настройки порта */
	DCB mst_dcb;
	/** @brief	Структура, содержащая значения таймаутов порта*/
	COMMTIMEOUTS mst_commTimeouts;

	/** @brief	Асинхронные операции чтения */
	OVERLAPPED mst_overlappedRead;
	/** @brief	Асинхронные операции записи */
	OVERLAPPED mst_overlappedWrite;

	string m_portName;
	DWORD m_baudRate;
	WORD m_dataBits;
	WORD m_parity;
	WORD m_stopBit;

	BOOLEAN m_isOverlapped;

	/**
	* @brief	Сброс параметров установки
	*/
	void resetInitParams();

public:
	SerialPort();	//конструктор по-умолчанию
	~SerialPort();	//деструктор по-умолчанию

	/**
	 * @brief	Получение текущего handle
	 * @return	The handle.
	 */
	HANDLE getHandle() { return m_Handle; }

	/**
	 * @brief	Установить параметры от другого порта (Должен быть HANDLE)
	 * @param [in,out]	otherPort	HANDLE другого порта
	 */
	void attach(HANDLE& otherPort);

	/**
	 * @brief	Проверка порта на валидность
	 * @return	True если handle не равен INVALID_HANDLE_VALUE, false в противном случае
	 */
	BOOL isHandleValid();

	//TODO: сделать bool
	/**
	 * @brief	Открытие порта и установка параметров
	 * @param	portName 	Имя порта строкой (без \\\\.\\)
	 * @param	baudrate 	Скорость в бодах
	 * @param	dataBits 	Количество бит данных
	 * @param	parity   	Контроль четности
	 * @param	stopBit  	Количество стоповых бит
	 * @param	asyncMode	Флаг асинхронной операции
	 */
	void open(std::string portName, DWORD baudrate, WORD dataBits, WORD parity, WORD stopBit, bool asyncMode);

	/**
	* @brief	Открытие порта и установка параметров
	* @param	portName 	Имя порта числом
	* @param	baudrate 	Скорость в бодах
	* @param	dataBits 	Количество бит данных
	* @param	parity   	Контроль четности
	* @param	stopBit  	Количество стоповых бит
	* @param	asyncMode	Флаг асинхронной операции
	*/
	void open(WORD portNumber, DWORD baudrate, WORD dataBits, WORD parity, WORD stopBit, bool asyncMode);

	/**
	 * @brief	Запись данных в порт
	 * @param	data		  	данные для записи
	 * @param	length		  	длина передаваемых данных
	 * @param	maxWaitTIme_ms	максимальное время ожидания (в миллисекундах) для асинхронной операции
	 * @return	Количество успешно переданных байт.
	 */

	int writeData(const unsigned char* data, UINT length, WORD maxWaitTIme_ms);

	/**
	 * @brief	Чтение данных из последовательного порта
	 * @param [in,out]	data		  	выходной буфер 
	 * @param 		  	length		  	длина буфера
	 * @param 		  	maxWaitTime_ms	максимальное время ожидания (в миллисекундах) для асинхронной операции
	 * @return	Количество успешно прочитанных байт
	 */
	int readData(unsigned char* data, UINT length, WORD maxWaitTime_ms);

	/**
	 * @fn	void SerialPort::getTimeouts();
	 * @brief	Gets the timeouts.
	 */
	void getTimeouts();
	COMMTIMEOUTS getTimeoutsToStruct();

	/**
	 * @brief	Sets the timeouts. Значение 0 - таймаут не используется
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
	 * @brief	Проверка на доступность порта
	 * @return	True if open, false if not.
	 */
	BOOL isOpen();

	/**
	 * @brief	Закрывает порт
	 * @todo  проверка на перекрываемые операции
	 */
	void close();

	/**
	 * @brief	Передать единичный символ в порт
	 * @warning Только синхронно
	 * @param	chr	передаваемый символ
	 * @todo	предупреждение компилятора, если пытаются вызвать асинхронно
	 */
	void transmitChar(unsigned char chr);			//только синхронно

	/**
	 * @brief	Очищает буферы порта и заодно осуществляет передачу всех данных из буфера на устройство
	 */
	void flushBuffers();

	/**
	 * @brief	Очистка порта
	 */
	void purge();

	/**
	 * @brief	Очистка буфера чтения
	 */
	void purgeReadBuffer();

	/**
	 * @brief	Очистка буфера записи
	 */
	void purgeWriteBuffer();

	/**
	 * @brief	Прервать асинхронные операции записи
	 * @param	purgeData	(Optional) Если True - очищает буфер записи
	 */
	void terminateWrite(BOOL purgeData = true);

	/**
	 * @brief	Прервать асинхронные операции чтения
	 * @param	purgeData	(Optional)  Если True - очищает буфер чтения
	 */
	void terminateRead(BOOL purgeData = true);

	/**
	 * @fn	void SerialPort::cancelIO();
	 * @brief	Отменяет все ожидающие операции ввода/вывода
	 */
	void cancelIO();

	/**
	 * @brief	Приостановка передачи данных для порта
	 * 			и установка линии передачи в состояние разрыва,
	 * 			пока ClearCommBreak функция не вызывается.
	 */
	void breakLine();

	/**
	 * @brief	Восстанавливает передачу данных для порта 
	 * 			и устанавливает линию передачи в нормальное состояние
	 */
	void restoreLine(); 


	///**
	// * @brief	Получает число символов в приемном буфере. 
	// * 			Эти символы приняты из линии, но еще не считаны функцией ReadFile;
	// * @return	Размер буфера чтения
	// * @warning у меня всегда возаращает ноль 
	// */
	//DWORD readBufferSize();
	//
	///**
	// * @brief	Получает число символов в передающем буфере. Эти символы еще не переданы в линию
	// * @return	Размер буфера записи
	// */
	//DWORD writeBufferSize();

	


	//TODO: потоки **ные и работа с ними

};

#endif	//_SERIAL_PORT_H_

