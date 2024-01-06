#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <cstdlib>
#include <vector>
struct FramBuffer
{
    size_t w, h; // 图片维度
    std::vector<uint32_t> img; // 存储图片信息
    void clear(const uint32_t color); // 清除图片
    void set_pixel(const size_t x, const size_t y, const uint32_t color); // 设置一个像素点颜色
    void draw_rectangle(const size_t x, const size_t y, const size_t w, const size_t h,
                    const uint32_t color); // 在图片里面设置一个map的方块
};

#endif