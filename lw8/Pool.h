#pragma once
#include "IWorker.h"
#include <vector>
#include <Windows.h>

DWORD WINAPI ThreadProc(LPVOID index)
{
	ITask* task = (ITask*)index;
	task->Execute();
	ExitThread(0);
}



class Pool : public IWorker
{

public:
	Pool(std::vector<ITask*>& tasks, size_t threadCount)
	{
		m_threadsCount = threadCount;
		for (size_t i = 0; i < tasks.size(); i++)
		{
			m_threads.push_back(CreateThread(NULL, i, &ThreadProc, tasks[i], CREATE_SUSPENDED, NULL));
		}
	}

	void ExecuteTasks()
	{
		int counter = 0;
		for (size_t i = 0; i < m_threads.size(); i++)
		{
			ResumeThread(m_threads[i]);
			counter++;

			if (m_threadsCount == counter)
			{
				WaitForMultipleObjects(i + 1, m_threads.data(), true, INFINITE);
				counter = 0;
			}
		}

		WaitForMultipleObjects(m_threadsCount, m_threads.data(), true, INFINITE);
	}


	bool IsBusy() 
	{
		return true;
	};


private:
	std::vector<HANDLE> m_threads;
	size_t m_threadsCount;
};


