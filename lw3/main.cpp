
#include "windows.h"
#include "tchar.h"

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>


struct ThreadParams
{
    unsigned threadIndex;
    unsigned operationCount;
    std::chrono::steady_clock::time_point startTime;
    std::string fileName;

    ThreadParams(unsigned index, unsigned opCount, std::string name, std::chrono::steady_clock::time_point start)
        : threadIndex(index)
        , operationCount(opCount)
        , fileName(name)
        , startTime(start)
    {}


    ThreadParams() {};
};

DWORD WINAPI ThreadProc(LPVOID index)
{
    ThreadParams* threadParams = (ThreadParams*)index;
    std::ofstream out(threadParams->fileName);


    for (size_t i = 0; i < 50; i++)
    {

        for (size_t i = 0; i < 10000000; i++)
        {
            int temp = 20 * 20 * 20 * 20 * 20 / 20 - 2000000 * 2222222222 % 2;
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - threadParams->startTime);
        out << std::to_string(elapsed.count()) + "\n" ;
    }


    ExitThread(0);
}

HANDLE* CreateThreads(unsigned threadCount, unsigned operatonCount, std::vector<std::string> names, std::chrono::steady_clock::time_point& startTime)
{
    HANDLE* handles = new HANDLE[threadCount];
    ThreadParams* params = new ThreadParams[threadCount];


    for (unsigned i = 0; i < threadCount; i++)
    {   
        params[i] = ThreadParams(i + 1, 100, names[i], startTime);
        handles[i] = CreateThread(NULL, 0, &ThreadProc, &params[i], CREATE_SUSPENDED, NULL);
    }

    return handles;
}

void LaunchThreads(HANDLE* const& handles, unsigned count) {

    for (size_t i = 0; i < count; i++)
    {
        ResumeThread(handles[i]);
    }
}

int main(int argc, char* argv[])
{

    int i;
    std::cin >> i;

    auto startTime = std::chrono::high_resolution_clock::now();
    auto handles = CreateThreads(2, 30, {"firstThreadOut.txt", "secondThreadOut.txt"}, startTime);
    LaunchThreads(handles, 2);

    WaitForMultipleObjects(2, handles, true, INFINITE);

    return 0;
}