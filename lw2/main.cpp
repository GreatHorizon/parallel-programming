#include <iostream>
#include <fstream>
#include <algorithm>
#include <optional>
#include <chrono>
#include <string>

#define _USE_MATH_DEFINES
#define NOMINMAX
#include <math.h>



#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

typedef struct
{
    uint8_t r, g, b, a;
} rgb32;


#if !defined(_WIN32) && !defined(_WIN64)
#pragma pack(2)
typedef struct
{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BITMAPFILEHEADER;
#pragma pack()


#pragma pack(2)
typedef struct
{
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int16_t biXPelsPerMeter;
    int16_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFOHEADER;
#pragma pack()
#endif

#pragma pack(2)
typedef struct
{
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
} BMPINFO;
#pragma pack()


class Bitmap
{
private:
    BMPINFO bmpInfo;
    uint8_t* pixels;

public:
    Bitmap(const char* path);
    ~Bitmap();

    void save(const char* path, uint16_t bit_count = 24);

    rgb32* getPixel(uint32_t x, uint32_t y) const;
    void setPixel(rgb32* pixel, uint32_t x, uint32_t y);

    uint32_t getWidth() const;
    uint32_t getHeight() const;
    uint16_t bitCount() const;
};

Bitmap::Bitmap(const char* path) : bmpInfo(), pixels(nullptr)
{
    std::ifstream file(path, std::ios::in | std::ios::binary);

    if (file)
    {
        file.read(reinterpret_cast<char*>(&bmpInfo.bfh), sizeof(bmpInfo.bfh));

        if (bmpInfo.bfh.bfType != 0x4d42)
        {
            throw std::runtime_error("Invalid format. Only bitmaps are supported.");
        }

        file.read(reinterpret_cast<char*>(&bmpInfo.bih), sizeof(bmpInfo.bih));

        if (bmpInfo.bih.biCompression != 0)
        {
            std::cerr << bmpInfo.bih.biCompression << "\n";
            throw std::runtime_error("Invalid bitmap. Only uncompressed bitmaps are supported.");
        }

        if (bmpInfo.bih.biBitCount != 24 && bmpInfo.bih.biBitCount != 32)
        {
            throw std::runtime_error("Invalid bitmap. Only 24bit and 32bit bitmaps are supported.");
        }

        file.seekg(bmpInfo.bfh.bfOffBits, std::ios::beg);

        pixels = new uint8_t[bmpInfo.bfh.bfSize - bmpInfo.bfh.bfOffBits];
        file.read(reinterpret_cast<char*>(&pixels[0]), bmpInfo.bfh.bfSize - bmpInfo.bfh.bfOffBits);


        uint8_t* temp = new uint8_t[bmpInfo.bih.biWidth * bmpInfo.bih.biHeight * sizeof(rgb32)];

        uint8_t* in = pixels;
        rgb32* out = reinterpret_cast<rgb32*>(temp);
        int padding = bmpInfo.bih.biBitCount == 24 ? ((bmpInfo.bih.biSizeImage - bmpInfo.bih.biWidth * bmpInfo.bih.biHeight * 3) / bmpInfo.bih.biHeight) : 0;

        for (int i = 0; i < bmpInfo.bih.biHeight; ++i, in += padding)
        {
            for (int j = 0; j < bmpInfo.bih.biWidth; ++j)
            {

                out->b = *(in++);
                out->g = *(in++);
                out->r = *(in++);
                out->a = bmpInfo.bih.biBitCount == 32 ? *(in++) : 0xFF;
                ++out;
            }
        }

        delete[] pixels;
        pixels = temp;
    }
}

Bitmap::~Bitmap()
{
    delete[] pixels;
}

void Bitmap::save(const char* path, uint16_t bit_count)
{
    std::ofstream file(path, std::ios::out | std::ios::binary);

    if (file)
    {
        bmpInfo.bih.biBitCount = bit_count;
        uint32_t size = ((bmpInfo.bih.biWidth * bmpInfo.bih.biBitCount + 31) / 32) * 4 * bmpInfo.bih.biHeight;
        bmpInfo.bfh.bfSize = bmpInfo.bfh.bfOffBits + size;

        file.write(reinterpret_cast<char*>(&bmpInfo.bfh), sizeof(bmpInfo.bfh));
        file.write(reinterpret_cast<char*>(&bmpInfo.bih), sizeof(bmpInfo.bih));
        file.seekp(bmpInfo.bfh.bfOffBits, std::ios::beg);

        uint8_t* out = NULL;
        rgb32* in = reinterpret_cast<rgb32*>(pixels);
        uint8_t* temp = out = new uint8_t[bmpInfo.bih.biWidth * bmpInfo.bih.biHeight * sizeof(rgb32)];
        int padding = bmpInfo.bih.biBitCount == 24 ? ((bmpInfo.bih.biSizeImage - bmpInfo.bih.biWidth * bmpInfo.bih.biHeight * 3) / bmpInfo.bih.biHeight) : 0;

        for (int i = 0; i < bmpInfo.bih.biHeight; ++i, out += padding)
        {
            for (int j = 0; j < bmpInfo.bih.biWidth; ++j)
            {
                *(out++) = in->b;
                *(out++) = in->g;
                *(out++) = in->r;

                if (bmpInfo.bih.biBitCount == 32)
                {
                    *(out++) = in->a;
                }
                ++in;
            }
        }

        file.write(reinterpret_cast<char*>(&temp[0]), size); //bmpInfo.bfh.bfSize - bmpInfo.bfh.bfOffBits
        delete[] temp;
    }
}

rgb32* Bitmap::getPixel(uint32_t x, uint32_t y) const
{
    rgb32* temp = reinterpret_cast<rgb32*>(pixels);
    return &temp[(bmpInfo.bih.biHeight - 1 - y) * bmpInfo.bih.biWidth + x];
}

void Bitmap::setPixel(rgb32* pixel, uint32_t x, uint32_t y)
{
    rgb32* temp = reinterpret_cast<rgb32*>(pixels);
    memcpy(&temp[(bmpInfo.bih.biHeight - 1 - y) * bmpInfo.bih.biWidth + x], pixel, sizeof(rgb32));
};

uint32_t Bitmap::getWidth() const
{
    return bmpInfo.bih.biWidth;
}

uint32_t Bitmap::getHeight() const
{
    return bmpInfo.bih.biHeight;
}

uint16_t Bitmap::bitCount() const
{
    return bmpInfo.bih.biBitCount;
}

struct ThreadParams
{
    uint32_t startWidth;
    uint32_t endWidth;
    uint32_t startHeight;
    uint32_t endHeight;
    uint32_t threadNumber;
    uint32_t rectangleHeight;
    uint32_t rectangleWidth;
    uint32_t extraHeight;
    uint32_t extraWidth;

    int threadsCount;
    Bitmap* bitmap;


    ThreadParams(uint32_t startW, uint32_t endW, uint32_t startH, 
        uint32_t endH, uint32_t rectangleW, uint32_t rectangleH, uint32_t extraW, uint32_t extraH, uint32_t number, int count, Bitmap* bmp)
        : startWidth(startW)
        , startHeight(startH)
        , endHeight(endH)
        , endWidth(endW)
        , extraWidth(extraW)
        , extraHeight(extraH)
        , threadNumber(number)
        , rectangleWidth(rectangleW)
        , rectangleHeight(rectangleH)
        , threadsCount(count)
        , bitmap(bmp)
        
    {};

    ThreadParams() {};

};

void blur(int radius, ThreadParams params)
{
    float rs = ceil(radius * 2.57);

    for (int i = params.startHeight ; i < params.endHeight; ++i)
    {
        for (int j = params.startWidth; j < params.endWidth; ++j)
        {
            double r = 0, g = 0, b = 0;
            double count = 0;

            for (int iy = i - rs; iy < i + rs + 1; ++iy)
            {
                for (int ix = j - rs; ix < j + rs + 1; ++ix)
                {
                    auto x = std::min(static_cast<int>(params.endWidth) - 1, std::max(0, ix));
                    auto y = std::min(static_cast<int>(params.endHeight) - 1, std::max(0, iy));

                    auto dsq = ((ix - j) * (ix - j)) + ((iy - i) * (iy - i));
                    auto wght = std::exp(-dsq / (2.0 * radius * radius)) / (M_PI * 2.0 * radius * radius);

                    rgb32* pixel = params.bitmap->getPixel(x, y);

                    r += pixel->r * wght;
                    g += pixel->g * wght;
                    b += pixel->b * wght;
                    count += wght;
                }
            }

            rgb32* pixel = params.bitmap->getPixel(j, i);
            pixel->r = std::round(r / count);
            pixel->g = std::round(g / count);
            pixel->b = std::round(b / count);
        }
    }
}


struct Args
{
    unsigned long threadsCount;
    unsigned long coresCount;
};

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
        throw std::invalid_argument("Numver should not contain letters");
    }

    return number;
}

Args ParseArgs(int argc, const char* argv[])
{
    if (argc != 5)
    {
        throw std::invalid_argument("Invalid argument count");
    }


    Args args;
    args.threadsCount = ConvertDigitStringIntoNumber(argv[3]);
    args.coresCount = ConvertDigitStringIntoNumber(argv[4]);

    if (args.threadsCount < 0 || args.coresCount < 0)
    {
        throw std::invalid_argument("Threads and cores count should be more than 0");
    }

    return args;
}

DWORD WINAPI ThreadProc(LPVOID params)
{
    ThreadParams* threadParams = (ThreadParams*)params;
    for (int i = 0; i < threadParams->threadsCount; i++)
    {
        threadParams->startWidth = threadParams->rectangleWidth * i;
        threadParams->endWidth = threadParams->rectangleWidth * (i + 1) + 
            (i == threadParams->threadsCount - 1 ? threadParams->extraWidth : 0);
        threadParams->startHeight = threadParams->rectangleHeight * threadParams->threadNumber;
        threadParams->endHeight = threadParams->rectangleHeight * (threadParams->threadNumber + 1) 
            + (threadParams->threadNumber == threadParams->threadsCount - 1 ? threadParams->extraHeight : 0);

        blur(5, *threadParams);
    }

    ExitThread(0);
}


void LaunchThreads(HANDLE* const& handles, unsigned count) {

    for (size_t i = 0; i < count; i++)
    {
        ResumeThread(handles[i]);
    }
}

void RunThreads(Args const& args, Bitmap &bmp)
{
    int squareHeight = bmp.getHeight() / args.threadsCount;
    int squareWidth = bmp.getWidth() / args.threadsCount;

    int extraHeight = bmp.getHeight() - squareHeight * args.threadsCount;
    int extraWidth = bmp.getWidth() - squareWidth * args.threadsCount;

    int startHeight = 0;
    int startWidth = 0;
    int endHeight = squareHeight;
    int endWidth = squareWidth;

    HANDLE* handles = new HANDLE[args.threadsCount];
    ThreadParams* threadParams = new ThreadParams[args.threadsCount];


    for (unsigned i = 0; i < args.threadsCount; i++)
    {
        threadParams[i] = ThreadParams(startWidth, endWidth, startHeight, endHeight, squareWidth, squareHeight,
            extraWidth, extraHeight, i, args.threadsCount, &bmp);
        
        handles[i] = CreateThread(NULL, 0, &ThreadProc, &threadParams[i], CREATE_SUSPENDED, NULL);
        SetThreadAffinityMask(handles[i], (1 << args.coresCount) - 1);
    }


    
    LaunchThreads(handles, args.threadsCount);
    WaitForMultipleObjects(args.threadsCount, handles, true, INFINITE);

}

int main(int argc, const char* argv[])
{
    try
    {
        auto args = ParseArgs(argc, argv);
        auto begin = std::chrono::high_resolution_clock::now();
        Bitmap bmp(argv[1]);

        RunThreads(args, bmp);
        bmp.save(argv[2]);
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        std::cout << "Execution time: " + std::to_string(elapsed.count());

    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << "\n";
        return 1;
    }



    return 0;
}