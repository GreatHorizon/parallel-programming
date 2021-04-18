
#include "windows.h"
#include <iostream>
#include <string>
#include "tchar.h"
#include <cstdlib>
#include <ctime>

CRITICAL_SECTION CriticalSection;

struct Arguments
{
    int m_workingVariable;


    Arguments(int workingVariable)
        : m_workingVariable(workingVariable)
    {}



    Arguments() {}
};

DWORD WINAPI ThreadProc(LPVOID index)
{
    int* variable = (int*)index;
    int localValue = *variable;
    std::srand(std::time(nullptr));
    int delta = (std::rand() % 10);

     //EnterCriticalSection(&CriticalSection);

    localValue += delta;
    *variable = localValue;

    std::cout << "workingVariable = " + std::to_string(*variable) + " localValue = " + std::to_string(localValue) + "\n";
    if (*variable != localValue)
    {
        std::cout << "Error. workingVariable = " + std::to_string(*variable) + " localValue = " + std::to_string(localValue) + "\n";
    }

   //LeaveCriticalSection(&CriticalSection);

    ExitThread(0);
}


HANDLE* CreateThreads(unsigned threadCount, int* args)
{
    HANDLE* handles = new HANDLE[threadCount];


    for (unsigned i = 0; i < threadCount; i++)
    {
        handles[i] = CreateThread(NULL, 0, &ThreadProc, args, CREATE_SUSPENDED, NULL);
        SetThreadAffinityMask(handles[i], (1 << 2) - 1);
    }

    return handles;
}

void LaunchThreads(HANDLE* const& handles, unsigned count) {

    for (size_t i = 0; i < count; i++)
    {
        ResumeThread(handles[i]);
    }
}

int main()
{

    //int i;
    //std::cin >> i;

    if (!InitializeCriticalSectionAndSpinCount(&CriticalSection,
        0x00000400))
        return 1;


    int* variable = new int(1);
    auto handles = CreateThreads(2, variable);
    LaunchThreads(handles, 2);

    WaitForMultipleObjects(2, handles, true, INFINITE);


    DeleteCriticalSection(&CriticalSection);
	return 0;
}