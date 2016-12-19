#include "SerialPort.h"

void SerialPort::resetInitParams()
{
	m_Handle = INVALID_HANDLE_VALUE;
	memset(&mst_comStat, 0, sizeof(COMSTAT));
	memset(&mst_commConfig, 0, sizeof(COMMCONFIG));
	memset(&mst_dcb, 0, sizeof(DCB));
	memset(&mst_commTimeouts, 0, sizeof(COMMTIMEOUTS));
	memset(&mst_overlappedRead, 0, sizeof(OVERLAPPED));
	memset(&mst_overlappedWrite, 0, sizeof(OVERLAPPED));

	m_portName = "";
	m_baudRate = 0;
	m_dataBits = 0;
	m_parity = 0;
	m_stopBit = 0;
	m_isOverlapped = false;
}

SerialPort::SerialPort()
{
	resetInitParams();
}

SerialPort::~SerialPort()
{

}

void SerialPort::checkHandleValid()
{
	if (m_Handle == INVALID_HANDLE_VALUE)
		throw (Exception(GetLastError()));
}

void SerialPort::open(std::string portName, DWORD baudrate, WORD dataBits, WORD parity, WORD stopBit, bool asyncMode)
{
	m_isOverlapped = asyncMode;
	m_portName = "\\\\.\\" + portName;
	m_Handle = CreateFile(m_portName.c_str(), GENERIC_WRITE | GENERIC_READ, NULL, NULL, OPEN_EXISTING, m_isOverlapped ? FILE_FLAG_OVERLAPPED : 0, 0);

	if (m_Handle == INVALID_HANDLE_VALUE) {
		ERR_MESSAGE("[CreateFile]");
		throw (Exception(GetLastError()));
	}

	DWORD dwSize = sizeof(COMMCONFIG);

	if (!GetCommConfig(m_Handle, &mst_commConfig, &dwSize)) {
		ERR_MESSAGE("[GetCommConfig]");
		throw (Exception(GetLastError()));
	}
	mst_commConfig.dcb.BaudRate = baudrate;
	mst_commConfig.dcb.ByteSize = dataBits;
	mst_commConfig.dcb.Parity = parity;
	mst_commConfig.dcb.StopBits = stopBit;

	if (!SetCommConfig(m_Handle, &mst_commConfig, dwSize)) {
		ERR_MESSAGE("[SetCommConfig]");
		throw (Exception(GetLastError()));
	}

	if (m_isOverlapped) {
		mst_overlappedRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		mst_overlappedWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	setTimeouts(0, 0, 0, 0, 0);


}
void SerialPort::open(WORD portNumber, DWORD baudrate, WORD dataBits, WORD parity, WORD stopBit, bool asyncMode)
{
	std::string buf = "COM" + std::to_string(portNumber);

	open("COM" + std::to_string(portNumber), baudrate, dataBits, parity, stopBit, asyncMode);
}

BOOL SerialPort::isOpen()
{
	return (m_Handle == INVALID_HANDLE_VALUE) ? false : true;
}

void SerialPort::close()
{
	//TODO:проверка на перекрываемые операции
	if (isOpen()) {
		CloseHandle(m_Handle);
		m_Handle = INVALID_HANDLE_VALUE;
	}
}

BOOL SerialPort::writeOneChar(UCHAR oneChar, WORD maxWaitTime_ms)
{
	if (!isOpen()) {
		ERR_MESSAGE("[Port is not open!]");
		throw (Exception(GetLastError()));
	}

	BOOL writeResult = false;
	DWORD bytesWritten = 0;

	writeResult = WriteFile(m_Handle, &oneChar, 1, &bytesWritten, &mst_overlappedWrite);
	if (!writeResult) {
		if (WaitForSingleObject(mst_overlappedWrite.hEvent, maxWaitTime_ms) != WAIT_OBJECT_0) {
			bytesWritten = 0;
			return false;
		}
		else {
			GetOverlappedResult(m_Handle, &mst_overlappedWrite, &bytesWritten, TRUE);
			mst_overlappedWrite.Offset += bytesWritten;
			return true;
		}
	}
}

BOOL SerialPort::sendData(UCHAR* data, UINT length)
{
	if (!isOpen()) {
		ERR_MESSAGE("[Port is not open!]");
		throw (Exception(GetLastError()));
	}
	PurgeComm(m_Handle, PURGE_TXCLEAR);

	BOOL isOk = false;
	DWORD bytesWritten = 0;
	int i;
	for (int i = 0; i < length; i++) {
		if (writeOneChar(data[i], 100))
			bytesWritten++;
		else {
			ERR_MESSAGE("[sendData:] не записался символ с индексом" + i);
		}
	}
	if (bytesWritten == length)
		isOk = true;

	return isOk;
}

int SerialPort::readData(unsigned char* data, UINT length, WORD maxWaitTime_ms)
{
	if (!isOpen())
	{
		ERR_MESSAGE("[Port is not open!]");
		throw (Exception(GetLastError()));
	}

	DWORD bytesRead;
	DWORD waitResult;

	//mst_overlappedRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	BOOL readResult = ReadFile(m_Handle, data, length, &bytesRead, &mst_overlappedRead);
	//при успешном старте overlapped операций readfile GetLastError() будет равен ERROR_IO_PENDING
	if (!readResult) {
		if (GetLastError() == ERROR_IO_PENDING)		{
			//cout << "ReadFile вернул ERROR_IO_PENDING, асинхронная операция стартовала" << endl;
		}

		waitResult = WaitForSingleObject(mst_overlappedRead.hEvent, maxWaitTime_ms);
		if (waitResult == WAIT_OBJECT_0)
		{
			//cout << "WaitForSingleObject вернул WAIT_OBJECT_0! все ок" << endl;
			if (GetOverlappedResult(m_Handle, &mst_overlappedRead, &bytesRead, FALSE)){
				//cout << "GetOverlappedResult завершена успешно" << endl;
			}
			else if (GetLastError() == ERROR_IO_INCOMPLETE) {
				cout << "GetOverlappedResult: GetLastError ERROR_IO_INCOMPLETE" << endl;
			}
		}
		if (waitResult == WAIT_TIMEOUT) {
			cout << "WaitForSingleObject - WAIT_TIMEOUT " << endl;
		}
		if (waitResult == WAIT_FAILED){
			cout << "WaitForSingleObject - WAIT_FAILED " << endl;
		}
		if (waitResult == WAIT_ABANDONED){
			cout << "WaitForSingleObject - WAIT_ABANDONED " << endl;
		}
	}
	else {
		cout << "ReadFile вернул " << readResult << endl;
	}

	return bytesRead;
}

void SerialPort::clear()
{
	if (!isOpen())
	{
		ERR_MESSAGE("[Port is not open!]");
		throw (Exception(GetLastError()));
	}

	PurgeComm(m_Handle, PURGE_RXCLEAR | PURGE_TXCLEAR);
}

void SerialPort::getTimeouts()
{
	GetCommTimeouts(m_Handle, &mst_commTimeouts);
	cout << "ReadIntervalTimeout\t" << mst_commTimeouts.ReadIntervalTimeout << endl;
	cout << "ReadTotalTimeoutConstant\t" << mst_commTimeouts.ReadTotalTimeoutConstant << endl;
	cout << "ReadTotalTimeoutMultiplier\t" << mst_commTimeouts.ReadTotalTimeoutMultiplier << endl;
	cout << "WriteTotalTimeoutConstant\t" << mst_commTimeouts.WriteTotalTimeoutConstant << endl;
	cout << "WriteTotalTimeoutMultiplier\t" << mst_commTimeouts.WriteTotalTimeoutMultiplier << endl;
}

void SerialPort::setTimeouts(UINT ReadIntervalTimeout, UINT ReadTotalTimeoutConstant,
	UINT ReadTotalTimeoutMultiplier, UINT WriteTotalTimeoutConstant,
	UINT WriteTotalTimeoutMultiplier)
{
	//timeouts
	GetCommTimeouts(m_Handle, &mst_commTimeouts);
	mst_commTimeouts.ReadIntervalTimeout = ReadIntervalTimeout;
	mst_commTimeouts.ReadTotalTimeoutConstant = ReadTotalTimeoutConstant;			//0 - не используется
	mst_commTimeouts.ReadTotalTimeoutMultiplier = ReadTotalTimeoutMultiplier;
	mst_commTimeouts.WriteTotalTimeoutConstant = WriteTotalTimeoutConstant;
	mst_commTimeouts.WriteTotalTimeoutMultiplier = WriteTotalTimeoutMultiplier;

	SetCommTimeouts(m_Handle, &mst_commTimeouts);
}

void SerialPort::transmitChar(unsigned char chr)
{
	if (!isOpen())
	{
		ERR_MESSAGE("[Port is not open!]");
		throw (Exception(GetLastError()));
	}

	if (!TransmitCommChar(m_Handle, chr))
	{
		ERR_MESSAGE("[TransmitCommChar]");
		throw (Exception(GetLastError()));
	}

}
void SerialPort::flushBuffers()
{
	if (!isOpen())
	{
		ERR_MESSAGE("[Port is not open!]");
		throw (Exception(GetLastError()));
	}

	if (!FlushFileBuffers(m_Handle))
	{
		ERR_MESSAGE("[flush]");
		throw (Exception(GetLastError()));
	}
}

void SerialPort::purge()
{
	if (!isOpen())
	{
		ERR_MESSAGE("[Port is not open!]");
		throw (Exception(GetLastError()));
	}

	if (!PurgeComm(m_Handle, PURGE_TXCLEAR | PURGE_TXCLEAR))
	{
		ERR_MESSAGE("[purge]");
		throw (Exception(GetLastError()));
	}
}

void SerialPort::purgeReadBuffer()
{
	if (!isOpen())
	{
		ERR_MESSAGE("[Port is not open!]");
		throw (Exception(GetLastError()));
	}

	if (!PurgeComm(m_Handle, PURGE_RXCLEAR))
	{
		ERR_MESSAGE("[purgeReadBuffer]");
		throw (Exception(GetLastError()));
	}
}
void SerialPort::purgeWriteBuffer()
{
	if (!isOpen())
	{
		ERR_MESSAGE("[Port is not open!]");
		throw (Exception(GetLastError()));
	}

	if (!PurgeComm(m_Handle, PURGE_TXCLEAR))
	{
		ERR_MESSAGE("[purgeReadBuffer]");
		throw (Exception(GetLastError()));
	}
}

void SerialPort::terminateWrite()
{
	if (!isOpen())
	{
		ERR_MESSAGE("[Port is not open!]");
		throw (Exception(GetLastError()));
	}

	if (!PurgeComm(m_Handle, PURGE_TXABORT))
	{
		ERR_MESSAGE("[terminateWrite]");
		throw (Exception(GetLastError()));
	}

}

void SerialPort::terminateRead()
{
	if (!isOpen())
	{
		ERR_MESSAGE("[Port is not open!]");
		throw (Exception(GetLastError()));
	}

	if (!PurgeComm(m_Handle, PURGE_RXABORT))
	{
		ERR_MESSAGE("[terminateRead]");
		throw (Exception(GetLastError()));
	}
}

DWORD SerialPort::getReadBufferSize()
{
	if (!isOpen())
	{
		ERR_MESSAGE("[Port is not open!]");
		throw (Exception(GetLastError()));
	}
	DWORD dwErrors;
	ClearCommError(m_Handle, &dwErrors, &mst_comStat);
	return mst_comStat.cbInQue;

}

DWORD SerialPort::getWriteBufferSize()
{
	if (!isOpen())
	{
		ERR_MESSAGE("[Port is not open!]");
		throw (Exception(GetLastError()));
	}

	DWORD dwErrors;
	ClearCommError(m_Handle, &dwErrors, &mst_comStat);
	return mst_comStat.cbOutQue;
}

void SerialPort::cancelIO()
{
	if (!isOpen())
	{
		ERR_MESSAGE("[Port is not open!]");
		throw (Exception(GetLastError()));
	}

	if (!CancelIo(m_Handle))
	{
		ERR_MESSAGE("[cancelIO]");
		throw (Exception(GetLastError()));
	}
}
void SerialPort::setBreak()
{
	if (!isOpen())
	{
		ERR_MESSAGE("[Port is not open!]");
		throw (Exception(GetLastError()));
	}

	if (!SetCommBreak(m_Handle))
	{
		ERR_MESSAGE("[setBreak]");
		throw (Exception(GetLastError()));
	}
}

void SerialPort::setUnbreak()
{
	if (!isOpen())
	{
		ERR_MESSAGE("[Port is not open!]");
		throw (Exception(GetLastError()));
	}
	if (!ClearCommBreak(m_Handle))
	{
		ERR_MESSAGE("[setUnbreak]");
		throw (Exception(GetLastError()));
	}
}




