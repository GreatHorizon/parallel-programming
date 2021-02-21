#include "windows.h"
#include "tchar.h"

#include <stdexcept>
#include <string>
#include <iostream>
#include <optional>
#include <memory>

DWORD WINAPI ThreadProc(LPVOID index)
{
    unsigned* threadIndex = (unsigned*)index;
    std::cout << "Working thread number " + std::to_string(*threadIndex) + "\n";
    delete[] threadIndex;
    ExitThread(0);
}

std::optional<unsigned> ConvertDigitStringIntoNumber(const std::string& valueLine)
{
    double number = 0;
    size_t stoppedAt;
    try
    {
        number = stoul(valueLine, &stoppedAt);
    }
    catch (const std::exception&)
    {
        return std::nullopt;
    }

    if (valueLine[stoppedAt] != '\0')
    {
        return std::nullopt;
    }

    return number;
}

unsigned CheckArguments(int count, char* arguments[]) 
{
    if (count != 2)
    {
        throw std::invalid_argument("Invalid argument count");
    }

    auto threadCount = ConvertDigitStringIntoNumber(arguments[1]);

    if (!threadCount)
    {
        throw std::invalid_argument("Thread count should be number");
    }

    if (threadCount.value() < 0)
    {
        throw std::invalid_argument("Thread count should be more than 0");
    }

    return threadCount.value();
}

HANDLE* CreateThreads(unsigned count)
{
    HANDLE* handles = new HANDLE[count];

    for (unsigned i = 0; i < count; i++)
    {   
        int* index = new int(i + 1);
        handles[i] = CreateThread(NULL, 0, &ThreadProc, index, CREATE_SUSPENDED, NULL);
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
    unsigned threadCount;
    try
    {
        threadCount = CheckArguments(argc, argv);
    }
    catch (const std::invalid_argument& e)
    {
        std::cout << e.what() << "\n";
        return 1;
    }

    auto handles = CreateThreads(threadCount);
    LaunchThreads(handles, threadCount);
    WaitForMultipleObjects(threadCount, handles, true, INFINITE);

    return 0;

}
