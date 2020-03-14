#pragma once
#include "ElectroWin.h"


void InitComms(HANDLE hComm) {
	hComm = CreateFile("COM4",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	dcbSerialParams.BaudRate = CBR_9600;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
}
void(*myPTR)(char);
void intit(void(*_getVal)(char))
{
	myPTR = _getVal;
}
/*
void put(uint8_t c) {
	char charBuf[] = { c };
	DWORD bytes;
	DWORD numWritten = 0;
	bytes = sizeof(c);
	myPTR(bytes);
	WriteFile(hComm, charBuf, bytes, &numWritten, NULL);
}
*/