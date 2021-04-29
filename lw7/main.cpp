#include "Worker.h"
#include "Task.h"
#include <vector>
#include <memory>

unsigned long ConvertDigitStringIntoNumber(const std::string& valueLine)
{
    unsigned long number = 0;
    size_t stoppedAt;
    try
    {
        number = stoul(valueLine, &stoppedAt);
    }
    catch (const std::exception&)
    {
        throw;
    }

    if (valueLine[stoppedAt] != '\0')
    {
        throw std::invalid_argument("Number should not contain letters");
    }

    return number;
}

int main(int argc, const char** argv)
{
    unsigned tasksCount;

    try
    {
        tasksCount = ConvertDigitStringIntoNumber(argv[1]);

        CWorker worker;
        for (size_t i = 0; i < tasksCount; i++)
        {
            ITask* task = new CTask(i + 1);
            worker.ExecuteTask(task);
        }

    }
    catch (const std::exception& e)
    {
        std::cout << e.what();

    }



	return 0;
}