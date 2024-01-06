#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cassert>

#include "utils.h"
/**
 *  返回一个32位的四通道颜色无符号整数
*/
uint32_t pack_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255)
{
    // 一个4 byte的无符号整数，每一个byte表示一个属性
    return (a << 24) + (b << 16) + (g << 8) + r;
}
/**
 * 经给定颜色分解得到四个通道数据
*/
void unpack_color(const uint32_t &color, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a)
{
    // 从给定的一个颜色，将属性抽出来
    r = (color >> 0) & 255;
    g = (color >> 8) & 255;
    b = (color >> 16) & 255;
    a = (color >> 24) & 255;
}
/**
 * 将给定的颜色数组image以指定文件名filename保存, 存储到硬盘上
*/
void drop_ppm_image(const std::string filename, const std::vector<uint32_t> &image, const size_t w, const size_t h)
{
    assert(image.size() == w * h);
    std::ofstream ofs(filename, std::ios::binary);
    ofs << "P6\n"
        << w << " " << h << "\n255\n";
    for (size_t i = 0; i < h * w; ++i)
    {
        uint8_t r, g, b, a;
        unpack_color(image[i], r, g, b, a);
        ofs << static_cast<char>(r) << static_cast<char>(g) << static_cast<char>(b);
    }
    ofs.close();
}