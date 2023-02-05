#pragma once

#include "Logger.h"

Logger::Logger(const char* fileName)
{
	this->fileName = fileName;
}
Logger::~Logger() {}


Logger* Logger::GetLogger(int level)
{
	Logger* retValue = NULL;

	switch (level)
	{
	case 0:
		retValue = new Logger_0("Log_0.txt");
		break;
	case 1:
		retValue = new Logger_1("Log_1.txt");
		break;
	case 2:
		retValue = new Logger_2("Log_2.txt");
		break;

		return retValue;
	}
}

Logger_0::Logger_0(const char* fileName) : Logger(fileName) {}

Logger_1::Logger_1(const char* fileName) : Logger(fileName) {}

Logger_2::Logger_2(const char* fileName) : Logger(fileName) {}

