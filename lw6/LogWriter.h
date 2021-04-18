#pragma once
#include <ostream>
#include <fstream>
#include "LinkedList.h"
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

class LogWriter
{

public:
	LogWriter()
	{
		out = std::ofstream("loggerOut.txt");
	}

	void WriteLogs(LinkedList& memoryBuffer)
	{
		for (auto it : memoryBuffer)
		{
			out << it;
		}

		out << "Buffer flushed\n";
		memoryBuffer.Clear();
	}

private:
	std::ofstream out;
};