#pragma once

#include <windows.h>
#include <stdio.h>
#include "LogWriter.h"
#include "LinkedList.h"

const int BUFFER_SIZE = 20;

class LogBuffer
{
private:
	LogWriter logWriter;
	LinkedList memoryBuffer;
	HANDLE flushBufferEvent;
	HANDLE logWriterThread;
	CRITICAL_SECTION criticals_section;

public:

	LogBuffer()
	{
		InitializeCriticalSectionAndSpinCount(&criticals_section, 0x00000400);
		flushBufferEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("FlushBufferEvent"));
		logWriterThread = CreateThread(NULL, 0, &LogSizeMonitoring, (void*)this, 0, NULL);
	}

	LinkedList& GetMemoryBuffer() 
	{
		return memoryBuffer;
	}

	HANDLE GetEventHandler()
	{
		return flushBufferEvent;
	}

	~LogBuffer() {
		DeleteCriticalSection(&criticals_section);
	}


	void WriteLog(std::string& const data)
	{
		EnterCriticalSection(&criticals_section);

		if (memoryBuffer.GetSize() == BUFFER_SIZE)
		{
			SetEvent(flushBufferEvent);
			StartFlushBuffer();
		}

		memoryBuffer.Push(data);

		LeaveCriticalSection(&criticals_section);
	}

private:
	static DWORD WINAPI LogSizeMonitoring(LPVOID arg)
	{
		LogBuffer* buffer = (LogBuffer*)arg;

		DWORD dwWaitResult = WaitForSingleObject(buffer->GetEventHandler(), INFINITE);

		if (dwWaitResult == WAIT_OBJECT_0)
		{
			buffer->logWriter.WriteLogs(buffer->GetMemoryBuffer());
		}

		ExitThread(0);
	}

	void StartFlushBuffer()
	{
		DWORD dwWaitResult = WaitForSingleObject(logWriterThread, INFINITE);

		if (dwWaitResult == WAIT_OBJECT_0)
		{
			logWriterThread = CreateThread(NULL, 0, &LogSizeMonitoring, (void*)this, 0, NULL);
			ResetEvent(flushBufferEvent);
		}
	}
};

