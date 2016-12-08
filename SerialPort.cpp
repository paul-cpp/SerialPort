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
	close();
	m_isOverlapped = asyncMode;
	m_portName = "\\\\.\\" + portName;
	m_Handle = CreateFile(portName.c_str(), GENERIC_WRITE | GENERIC_READ, NULL, NULL, OPEN_EXISTING, m_baudRate ? FILE_FLAG_OVERLAPPED : 0, 0);

	if (m_Handle == INVALID_HANDLE_VALUE){
		ERR_MESSAGE("[CreateFile]");
		throw (Exception(GetLastError()));
	}

	DWORD dwSize = sizeof(COMMCONFIG);
	
	/**@remark вопрос по sizeof */
	if (!GetCommConfig(m_Handle, &mst_commConfig, &dwSize)) {
		ERR_MESSAGE("[GetCommConfig]");
		throw (Exception(GetLastError()));
	}
	mst_commConfig.dcb.BaudRate = baudrate;
	mst_commConfig.dcb.ByteSize = dataBits;
	mst_commConfig.dcb.Parity = parity;
	mst_commConfig.dcb.StopBits = stopBit;

	if (!SetCommConfig(m_Handle, &mst_commConfig, dwSize)){
		ERR_MESSAGE("[SetCommConfig]");
		throw (Exception(GetLastError()));
	}

	//finished here
}

bool SerialPort::isOpen()
{
	return (m_Handle == INVALID_HANDLE_VALUE) ? false : true;
}

void SerialPort::close()
{
	//TODO:проверка на перекрываемые операции
	if (isOpen())
	{
		CloseHandle(m_Handle);
		m_Handle = INVALID_HANDLE_VALUE;
	}
}
