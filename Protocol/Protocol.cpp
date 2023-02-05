#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <conio.h>
#include <map>
#include <string>
#include <thread>

#include "Logger.h"

//*******************************
// Логи в папке Protocol\Protocol
//*******************************

bool CreateUnnamedPipe(HANDLE &write, HANDLE &read, int bufSize)
{
	if (!CreatePipe(
		&read,
		&write,
		NULL,
		bufSize))
	{
		return false;
	}
	else
	{
		return true;
	}
}
bool ReadPipeNoWait(HANDLE read, char* buf)
{
	DWORD avail;

	if (!PeekNamedPipe(read, NULL, sizeof(buf), NULL, &avail, NULL))
	{
		printf("Error read 1\n");

		return false;
	}
	else
	{
		if (avail > 0)
		{
			if (!ReadFile(read, buf, sizeof(buf), NULL, NULL))
			{
				printf("Error read \n");

				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}
}
bool ReadPipeWait(HANDLE read, char* buf, int bufSize)
{
	if (!ReadFile(read, buf, bufSize, NULL, NULL))
	{
		printf("Error read \n");

		return false;
	}
	else
	{
		return true;
	}
}
bool WritePipe(HANDLE write, char* buf)
{
	if (!WriteFile(write, buf, strlen(buf) + 1, NULL, NULL))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool isRunning = true;
bool isPaused = false;

HANDLE FirstToSecond_Write_Pipe;
HANDLE FirstToSecond_Read_Pipe;

HANDLE FirstToThird_Write_Pipe;
HANDLE FirstToThird_Read_Pipe;

HANDLE SecondToFirst_Write_Pipe;
HANDLE SecondToFirst_Read_Pipe;

HANDLE SecondToThird_Write_Pipe;
HANDLE SecondToThird_Read_Pipe;

std::map<std::string, void (*)(const char*)> Commands;

void date(const char* params)
{
	time_t now = std::time(NULL);

	tm* ltm = localtime(&now);

	std::cout << "Year: " << 1900 + ltm->tm_year << std::endl;
	std::cout << "Month: " << 1 + ltm->tm_mon << std::endl;
	std::cout << "Day: " << ltm->tm_mday << std::endl;
	std::cout << "Time: " << 1 + ltm->tm_hour << ":";
	std::cout << 1 + ltm->tm_min << ":";
	std::cout << 1 + ltm->tm_sec << std::endl;
}
void time(const char* params)
{
	time_t now = std::time(NULL);

	tm* ltm = localtime(&now);

	std::cout << "Time: " << 1 + ltm->tm_hour << ":";
	std::cout << 1 + ltm->tm_min << ":";
	std::cout << 1 + ltm->tm_sec << std::endl;
}
void exit(const char* params)
{
	printf("Exitting\n");

	isRunning = false;
}
void faster(const char* params)
{
	char FirstToSecondPipe_Buf[32] = "faster";

	if (!WritePipe(FirstToSecond_Write_Pipe, FirstToSecondPipe_Buf))
	{
		printf("Error Data Transfer!\n");
	}
}
void slower(const char* params)
{
	char FirstToSecondPipe_Buf[32] = "slower";

	if (!WritePipe(FirstToSecond_Write_Pipe, FirstToSecondPipe_Buf))
	{
		printf("Error Data Transfer!\n");
	}
}
void pause(const char* params)
{
	char FirstToSecondPipe_Buf[32] = "pause";

	if (!WritePipe(FirstToSecond_Write_Pipe, FirstToSecondPipe_Buf))
	{
		printf("Error Data Transfer!\n");
	}
}
void resume(const char* params)
{
	char FirstToSecondPipe_Buf[32] = "resume";

	if (!WritePipe(FirstToSecond_Write_Pipe, FirstToSecondPipe_Buf))
	{
		printf("Error Data Transfer!\n");
	}
}
void level(const char* params)
{
	char FirstToThirdPipe_Buf[32] = "";

	if (std::atoi(params) > 2 || std::atoi(params) < 0)
	{
		std::cerr << "Error: level [0, 1, 2] " << std::endl;
	}
	else
	{
		strcat(FirstToThirdPipe_Buf, "level ");
		strcat(FirstToThirdPipe_Buf, params);

		if (!WritePipe(FirstToThird_Write_Pipe, FirstToThirdPipe_Buf))
		{
			printf("Error Data Transfer!\n");
		}
	}
}
void stats(const char* params)
{
	char FirstToThirdPipe_Buf[32] = "stats";

	if (!WritePipe(FirstToSecond_Write_Pipe, FirstToThirdPipe_Buf))
	{
		printf("Error Data Transfer!\n");
	}
	else
	{
		char RespFromSecond_Buf[32] = "";

		if(ReadPipeWait(SecondToFirst_Read_Pipe, RespFromSecond_Buf, sizeof(RespFromSecond_Buf)))
		{
			printf("Event count %s \n", RespFromSecond_Buf);
		}
	}
}

void HandleCommand(const char* command)
{
	const char* comm;
	const char* param;

	comm  = strtok((char*)command, " ");
	param = strtok(NULL, " ");

	if (Commands.find(comm) != Commands.end())
	{
		//printf("Command %s\n", comm);
		//printf("Params %s \n", param);

		Commands[command](param);
	}
	else
	{
		std::cerr << "Error: No such command " << std::endl;
	}	
}

void FirstThreadFun()
{
	printf("------ Commands -----\n date\n time\n exit\n faster\n slower\n pause\n resume\n level\n stats\n");

	while (isRunning)
	{
		std::string command;
		
		std::cout << "Command->> ";
		std::getline(std::cin, command);

		if (!command.empty())
		{
			HandleCommand(command.c_str());
		}
	}
}

void SecondThreadFun()
{
	//генерация событий
	int interval = 1000;

	int eventCount = 0;

	while (true)
	{
		char CommandFromFirstThread_Buf[32] = "";

		// принимаем команду resume, pause, slower, faster из первого потока
		{
			if(ReadPipeNoWait(FirstToSecond_Read_Pipe, CommandFromFirstThread_Buf))
			{
				std::string command = CommandFromFirstThread_Buf;

				if (command == "pause")
					isPaused = true;
				if (command == "resume")
					isPaused = false;
				if (command == "slower")
					interval += 200;
				if (command == "faster")
				{
					if (interval - 200 > 200)
					{
						interval -= 200;
					}
				}

				if (command == "stats")
				{
					char StastBuf[8] = "";

					strcat(StastBuf, std::to_string(eventCount).c_str());

					if (!WritePipe(SecondToFirst_Write_Pipe, StastBuf))
					{
						printf("Error Data Transfer!\n");
					}
				}
			}
		}

		if (!isPaused)
		{
			Event event((0 + rand() % 4),
				(0 + rand() % 99),
				(0 + rand() % 99),
				(0 + rand() % 99));

			char EventBuf[128] = "";

			strcat(EventBuf, event.now);
			strcat(EventBuf, "/");
			strcat(EventBuf, std::to_string(event.id).c_str());
			strcat(EventBuf, "/");
			strcat(EventBuf, std::to_string(event.param_0).c_str());
			strcat(EventBuf, "/");
			strcat(EventBuf, std::to_string(event.param_1).c_str());
			strcat(EventBuf, "/");
			strcat(EventBuf, std::to_string(event.param_2).c_str());

			if (!WritePipe(SecondToThird_Write_Pipe, EventBuf))
			{
				printf("Error Data Transfer!\n");
			}

			eventCount++;
		}	

		Sleep (0 + rand() % interval);
	}
}

void ThirdThreadFun()
{
	Logger* logger_0 = Logger::GetLogger(0);
	Logger* logger_1 = Logger::GetLogger(1);
	Logger* logger_2 = Logger::GetLogger(2);

	Logger* currentLogger = NULL;

	while (true)
	{
		char CommandFromFirstThread_Buf[32] = "";

		{ // принимаем команду level зи первого потока
			if (ReadPipeNoWait(FirstToThird_Read_Pipe, CommandFromFirstThread_Buf))
			{
				std::string command = CommandFromFirstThread_Buf;

				if (command.find("level", 0) != std::string::npos)
				{
					switch (command[6] - '0')
					{
					case 0:
						currentLogger = logger_0;
						break;
					case 1:
						currentLogger = logger_1;
						break;
					case 2:
						currentLogger = logger_2;
						break;
					}
				}
			}
		}

		//протоколируем
		char EventBuf[128] = " ";

		if (ReadPipeWait(SecondToThird_Read_Pipe, EventBuf, sizeof(EventBuf)))
		{
			const char* date = strtok(EventBuf, "/");
			const char* id = strtok(NULL, "/");
			const char* param_0 = strtok(NULL, "/");
			const char* param_1 = strtok(NULL, "/");
			const char* param_2 = strtok(NULL, "/");

			Event eventToLog;

			strcpy(eventToLog.now, date);
			eventToLog.id = std::atoi(id);
			eventToLog.param_0 = std::atoi(param_0);
			eventToLog.param_1 = std::atoi(param_1);
			eventToLog.param_2 = std::atoi(param_2);

			if (currentLogger != NULL)
				currentLogger->Write(eventToLog);		
		}			
	}
}

int main()
{
	Commands.insert({ "date",	date });
	Commands.insert({ "time",	time });
	Commands.insert({ "exit",	exit });
	Commands.insert({ "faster", faster });
	Commands.insert({ "slower", slower });
	Commands.insert({ "pause",  pause });
	Commands.insert({ "resume", resume });
	Commands.insert({ "level",  level });
	Commands.insert({ "stats",  stats });

	//создаём каналы
	{
		if (!CreateUnnamedPipe(FirstToThird_Write_Pipe, FirstToThird_Read_Pipe, 128))
		{
			_cputs("Create pipe failed.\n");
			_cputs("Press any key to finish.\n");
			_getch();

			return 0;
		}
		if (!CreateUnnamedPipe(SecondToThird_Write_Pipe, SecondToThird_Read_Pipe, 128))
		{
			_cputs("Create pipe failed.\n");
			_cputs("Press any key to finish.\n");
			_getch();

			return 0;
		}
		if (!CreateUnnamedPipe(FirstToSecond_Write_Pipe, FirstToSecond_Read_Pipe, 128))
		{
			_cputs("Create pipe failed.\n");
			_cputs("Press any key to finish.\n");
			_getch();

			return 0;
		}
		if (!CreateUnnamedPipe(SecondToFirst_Write_Pipe, SecondToFirst_Read_Pipe, 128))
		{
			_cputs("Create pipe failed.\n");
			_cputs("Press any key to finish.\n");
			_getch();

			return 0;
		}
	}

	std::thread FirstThread(FirstThreadFun);
	std::thread SecondThread(SecondThreadFun);
	std::thread ThirdThread(ThirdThreadFun);

	FirstThread.join();
	SecondThread.join();
	ThirdThread.join();

	return 0;
}