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
	m_Handle = CreateFile(m_portName.c_str(), GENERIC_WRITE | GENERIC_READ, NULL, NULL, OPEN_EXISTING, m_baudRate ? FILE_FLAG_OVERLAPPED : 0, 0);

	if (m_Handle == INVALID_HANDLE_VALUE) {
		ERR_MESSAGE("[CreateFile]");
		cout << "file name\t" << m_portName << endl;
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

	//timeouts
	GetCommTimeouts(m_Handle, &mst_commTimeouts);
	mst_commTimeouts.ReadIntervalTimeout = 0;
	mst_commTimeouts.ReadTotalTimeoutConstant = 150;	//не используетс€
	mst_commTimeouts.ReadTotalTimeoutMultiplier = 0;
	mst_commTimeouts.WriteTotalTimeoutConstant = 0;
	mst_commTimeouts.WriteTotalTimeoutMultiplier = 0;

	SetCommTimeouts(m_Handle, &mst_commTimeouts);

	//SetupComm(m_Handle, 20, 20);

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
			ERR_MESSAGE("[sendData:] не записалс€ символ с индексом" + i);
		}
	}
	if (bytesWritten == length)
		isOk = true;

	return isOk;
}

int SerialPort::readData(void* data, UINT length, WORD maxWaitTime_ms)
{
	if (!isOpen()) {
		ERR_MESSAGE("[Port is not open!]");
		throw (Exception(GetLastError()));
	}

	PurgeComm(m_Handle, PURGE_RXCLEAR);

	BOOL readStatus = false;
	DWORD bytesRead = 0;
	DWORD errorFlags;
	DWORD fb = 0;

	bytesRead = length;

	readStatus = ReadFile(m_Handle, data, bytesRead, &fb, &mst_overlappedRead);
	if (readStatus == true) {
		if (WaitForSingleObject(mst_overlappedRead.hEvent, maxWaitTime_ms) == WAIT_OBJECT_0) {
		}
		else {
			cout << "--Ќ≈ успел" << endl;
		}
	}
	else if (GetLastError() == ERROR_IO_PENDING) {
		cout << "превышено ожидание чтени€" << endl;
	}

	return fb;

}
