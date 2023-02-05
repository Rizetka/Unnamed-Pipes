#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include <iostream>
#include <ctime>
#include <fstream>

struct Event
{
	char now[64];

	int id;

	int param_0;
	int param_1;
	int param_2;

	Event(int id, int param_0, int param_1, int param_2)
	{
		time_t n = std::time(NULL);

	    strcpy(now, ctime(&n));

		this->id = id;
		this->param_0 = param_0;
		this->param_1 = param_1;
		this->param_2 = param_2;
	}

	Event()
	{
		this->id = 0;
		this->param_0 = 0;
		this->param_1 = 0;
		this->param_2 = 0;
	}
};

class Logger
{
protected:
	std::string fileName;

	Logger(const char* fileName);
	virtual  ~Logger() = 0;

public:
	static Logger* GetLogger(int level);

	virtual void Write(Event e) const = 0;
};

class Logger_0 : public Logger
{	
public:
	Logger_0(const char* fileName);

	void Write(Event e) const override
	{
		std::ofstream out;    

		out.open(fileName, std::ios::app);

		if (out.is_open())
		{
			out << "Date:    " << e.now << std::endl;
			out << "ID:      " << e.id << std::endl;

			out << "=================================" << std::endl;
		}
	}
};

class Logger_1 : public Logger
{
public:
	Logger_1(const char* fileName);

	void Write(Event e) const override
	{
		std::ofstream out;

		out.open(fileName, std::ios::app);

		if (out.is_open())
		{
			out << "Date:    " << e.now << std::endl;
			out << "ID:      " << e.id << std::endl;
			out << "Param_0: " << e.param_0 << std::endl;

			out << "=================================" << std::endl;
		}
	}
};

class Logger_2 : public Logger
{
public:
	Logger_2(const char* fileName);

	void Write(Event e) const override
	{
		std::ofstream out;

		out.open(fileName, std::ios::app);

		if (out.is_open())
		{
			out << "Date:	 " << e.now << std::endl;
			out << "ID:		 " << e.id << std::endl;
			out << "Param_0: " << e.param_0 << std::endl;
			out << "Param_1: " << e.param_1 << std::endl;
			out << "Param_2: " << e.param_2 << std::endl;

			out << "=================================" << std::endl;
		}
	}
};
