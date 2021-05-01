#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include "Blur.h"
#include <filesystem>
#include "Pool.h"

#include "Bitmap.h"

#include "Utils.h"

struct Args
{
    std::string mode;
    unsigned long threadsCount;
    unsigned long blockCount;
    std::string inputDirectory;
    std::string outputDirectory;
};


Args ParseArgs(int argc, const char* argv[])
{
    Args args;

    args.mode = argv[1];
    args.threadsCount = ConvertDigitStringIntoNumber(argv[5]);
    args.blockCount = ConvertDigitStringIntoNumber(argv[2]);
    args.inputDirectory = argv[3];
    args.outputDirectory = argv[4];


    if (argc != 6)
    {
        throw std::invalid_argument("Invalid argument count");
    }

    return args;
}

//void RunThreads(Args const& args, Bitmap& bmp, std::chrono::steady_clock::time_point& startTime)
//{
//    int squareHeight = bmp.getHeight() / args.threadsCount;
//    int squareWidth = bmp.getWidth() / args.threadsCount;
//
//    std::cout << bmp.getHeight() << "\n";
//    std::cout << bmp.getWidth() << "\n";
//
//    int extraHeight = bmp.getHeight() - squareHeight * args.threadsCount;
//    int extraWidth = bmp.getWidth() - squareWidth * args.threadsCount;
//
//    int startHeight = 0;
//    int startWidth = 0;
//    int endHeight = squareHeight;
//    int endWidth = squareWidth;
//
//    HANDLE* handles = new HANDLE[args.threadsCount];
//    ThreadParams* threadParams = new ThreadParams[args.threadsCount];
//
//    for (unsigned i = 0; i < args.threadsCount; i++)
//    {
//
//        threadParams[i] = ThreadParams(startWidth, endWidth, startHeight, endHeight, squareWidth, squareHeight,
//            extraWidth, extraHeight, i, &bmp, startTime);
//        handles[i] = CreateThread(NULL, 0, &ThreadProc, &threadParams[i], CREATE_SUSPENDED, NULL);
//    }
//
//    LaunchThreads(handles, args.threadsCount);
//    WaitForMultipleObjects(args.threadsCount, handles, true, INFINITE);
//
//}
 
void LaunchThreads(HANDLE* const& handles, unsigned count) {

    for (size_t i = 0; i < count; i++)
    {
        ResumeThread(handles[i]);
    }
}


std::vector<ITask*> CreateTasks(Bitmap& bitmap, size_t blocksCount)
{
    std::vector<ITask*> tasks;

    int height = bitmap.getHeight();
    int width = bitmap.getWidth();

    int blockHeight = bitmap.getHeight() / blocksCount;
    int extraHeight = bitmap.getHeight() - blocksCount * blockHeight;


    uint32_t startWidth, endWidth, startHeight, endHeight;
    startWidth = 0;
    endWidth = bitmap.getWidth();
    for (size_t i = 0; i < blocksCount; i++)
    {

        startHeight = blockHeight * i;

        endHeight = blockHeight * (i + 1);
        if (i == blocksCount - 1)
        {
            endHeight += extraHeight;
        }

        auto params = new ThreadParams(startWidth, endWidth, startHeight, endHeight, &bitmap);

        auto task = new Blur(*params);

        tasks.push_back(task);
    }

    return tasks;
}


std::vector<std::filesystem::path> GetFile(std::string directoryName) 
{

    std::vector<std::filesystem::path> fileNames;
    for (auto& p : std::filesystem::directory_iterator(directoryName))
    {
        if (p.path().extension().string() == ".bmp" )
        {
            fileNames.push_back(p.path());
        }
    }

    return fileNames;
}

int main(int argc, const char* argv[])
{


    auto startTime = std::chrono::high_resolution_clock::now();
    try
    {
        auto args = ParseArgs(argc, argv);
        auto files = GetFile("in");


        for (auto& file : files)
        {
            Bitmap bmp(file.string().c_str());
            std::vector<ITask*> tasks = CreateTasks(bmp, args.blockCount);

            Pool pool(tasks, args.threadsCount);

            if (args.mode == "1")
            {
                pool.ExecuteTasks();
            }

            else if (args.mode == "2")
            {
                HANDLE* handles = new HANDLE[args.blockCount];
                for (int i = 0; i < args.blockCount; i++)
                {
                    handles[i] = CreateThread(NULL, 0, &ThreadProc, tasks[i], 0, NULL);
                }

                WaitForMultipleObjects(args.blockCount, handles, true, INFINITE);
            }
            else
            {
                std::cout << "Invalid mode\n";
            }

            std::string output = "out/" + file.filename().string();
            bmp.save(output.c_str());
        }


    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << "\n";
        return 1;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - startTime);
    std::cout << std::to_string(elapsed.count()) + "\n";

    return 0;
}