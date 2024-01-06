#include <iostream>
#include <vector>
#include <cstdint>
#include <cassert>

#include "framebuffer.h"
#include "utils.h"
/// @brief 设置一个像素点的颜色
/// @param x 
/// @param y 
/// @param color 
void FramBuffer::set_pixel(const size_t x, const size_t y, const uint32_t color){
    assert(img.size() == w * h && x < w && y < h);
    img[x + y * w] = color;
}
/// @brief 在像素图上画一个规定宽高的矩形
/// @param rect_x 
/// @param rect_y 
/// @param rect_w 
/// @param rect_h 
/// @param color 
void FramBuffer::draw_rectangle(const size_t rect_x, const size_t rect_y, const size_t rect_w, const size_t rect_h,
                    const uint32_t color){
    assert(img.size() == w * h);
    for(size_t i = 0; i < rect_w; ++i){
        for(size_t j = 0; j < rect_h; ++j){
            size_t cx = rect_x + i;
            size_t cy = rect_y + j;
            if(cx < w && cy < h){
                set_pixel(cx, cy, color);
            }
        }
    }
}
/// @brief 清空像素图
/// @param color 
void FramBuffer::clear(const uint32_t color){
    img = std::vector<uint32_t>(w * h, color);
}
