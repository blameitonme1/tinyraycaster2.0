#ifndef TEXTURES_H
#define TEXTURES_H
#include <cstdint>
#include <vector>
#include <string>
struct Texture
{
    size_t img_w, img_h; // 质地图片的宽度和高度
    size_t count, size; // 质地的数量和大小 (像素)
    std::vector<uint32_t> img; // 存储图片颜色像素
    Texture(const std::string filename);
    uint32_t get(const size_t i, const size_t j, const size_t id);
    std::vector<uint32_t> get_scaled_colum(const size_t texid, const size_t texcoord, const size_t column_height);
};

#endif