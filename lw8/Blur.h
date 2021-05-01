#pragma once
#include "ITask.h"
#include "Utils.h"
#include "Bitmap.h"

struct ThreadParams
{
    uint32_t startWidth;
    uint32_t endWidth;
    uint32_t startHeight;
    uint32_t endHeight;
    std::chrono::steady_clock::time_point startTime;

    Bitmap* bitmap;


    ThreadParams(uint32_t startW, uint32_t endW, uint32_t startH,
        uint32_t endH, Bitmap* bmp)
        : startWidth(startW)
        , startHeight(startH)
        , endHeight(endH)
        , endWidth(endW)
        , bitmap(bmp)

    {};

    ThreadParams() {};

};

class Blur : public ITask
{
public:

    Blur(ThreadParams params)
    {
        m_params = params;
    }

    void Execute() override
    {
        int radius = 5;
        float rs = ceil(radius * 2.57);

        for (int i = m_params.startHeight; i < m_params.endHeight; ++i)
        {
            for (int j = m_params.startWidth; j < m_params.endWidth; ++j)
            {
                double r = 0, g = 0, b = 0;
                double count = 0;

                for (int iy = i - rs; iy < i + rs + 1; ++iy)
                {
                    for (int ix = j - rs; ix < j + rs + 1; ++ix)
                    {
                        auto x = std::min(static_cast<int>(m_params.endWidth) - 1, std::max(0, ix));
                        auto y = std::min(static_cast<int>(m_params.endHeight) - 1, std::max(0, iy));

                        auto dsq = ((ix - j) * (ix - j)) + ((iy - i) * (iy - i));
                        auto wght = std::exp(-dsq / (2.0 * radius * radius)) / (M_PI * 2.0 * radius * radius);

                        rgb32* pixel = m_params.bitmap->getPixel(x, y);

                        r += pixel->r * wght;
                        g += pixel->g * wght;
                        b += pixel->b * wght;
                        count += wght;
                    }
                }

                rgb32* pixel = m_params.bitmap->getPixel(j, i);
                pixel->r = std::round(r / count);
                pixel->g = std::round(g / count);
                pixel->b = std::round(b / count);
            }
        }
    }

private:
    ThreadParams m_params;
};

