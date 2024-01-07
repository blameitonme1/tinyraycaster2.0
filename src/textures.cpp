#include <iostream>
#include <cassert>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "utils.h"
#include "textures.h"
#include <SDL2/SDL.h>
/// @brief 加载质地图片到内存
/// @param filename 
Texture::Texture(const std::string filename, const uint32_t format) : img_w(0), img_h(0), count(0), size(0),img(){
    SDL_Surface *tmp = SDL_LoadBMP(filename.c_str());
    if (!tmp) {
        std::cout << filename.c_str() << std::endl;
        std::cerr << "Error in SDL_LoadBMP: " << SDL_GetError() << std::endl;
        return;
    }
    SDL_Surface *surface = SDL_ConvertSurfaceFormat(tmp, format, 0);
    SDL_FreeSurface(tmp);
    if (!surface) {
        std::cerr << "Error in SDL_ConvertSurfaceFormat: " << SDL_GetError() << std::endl;
        return;
    }
    int w = surface->w;
    int h = surface->h;
    if (w * 4 != surface->pitch) {
        // 检查参数是否四个通道，32位
        std::cerr << "Error: the texture must be a 32 bit image" << std::endl;
        SDL_FreeSurface(surface);
        return;
    }
    if (w != h * int(w/h)) {
        std::cerr << "Error: the texture file must contain N square textures packed horizontally" << std::endl;
        SDL_FreeSurface(surface);
        return;
    }
    count = w / h;
    size = w / count;
    img_w = w;
    img_h = h;
    // 提取所有像素信息
    uint8_t *pixmap = reinterpret_cast<uint8_t*>(surface->pixels);
    img = std::vector<uint32_t>(w * h);
    for (int j=0; j<h; j++) {
        for (int i=0; i<w; i++) {
            // 转化成32位的颜色整数
            uint8_t r = pixmap[(i+j*w)*4+0];
            uint8_t g = pixmap[(i+j*w)*4+1];
            uint8_t b = pixmap[(i+j*w)*4+2];
            uint8_t a = pixmap[(i+j*w)*4+3];
            img[i+j*w] = pack_color(r, g, b, a);
        }
    }
    SDL_FreeSurface(surface);
    return;
}
/// @brief 获取指定点的颜色数据
/// @param i 
/// @param j 
/// @param id 
/// @return 
uint32_t Texture::get(const size_t i, const size_t j, const size_t id){
    assert(i < img_w && j < img_h && id < count);
    return img[i + id * size + j * img_w];
}
/// @brief 得到要求的column的颜色数据，以vector的形式返回
/// @param texid 
/// @param texcoord 
/// @param column_height 
/// @return 
std::vector<uint32_t> Texture::get_scaled_colum(const size_t texid, const size_t texcoord, const size_t column_height){
    assert(img.size() == img_w * img_h && texcoord < size && texid < count);
    std::vector<uint32_t> column(column_height);
    for(size_t y = 0; y < column_height; ++y){
        column[y] = get(texcoord, (y * size) / column_height, texid);
    }
    return column;
}