#include <iostream>
#include <cassert>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "utils.h"
#include "textures.h"
/// @brief 加载质地图片到内存
/// @param filename 
Texture::Texture(const std::string filename) : img_w(0), img_h(0), count(0), size(0),img(){
    int nchannels = -1, w, h;
    // 从图片加载墙面质地和怪物质地
    unsigned char *pixmap = stbi_load(filename.c_str(), &w, & h, &nchannels, 0);
    if(!pixmap){
        std::cerr << "Error: cannot load the textures" << std::endl;
        return;
    }
    if(nchannels != 4){
        std::cerr << "Eoor: the texture must be 32 bit image" << std::endl;
        stbi_image_free(pixmap);
        return;
    }
    count = w / h; // 有多少个质地
    size = w / count; // 一个质地的大小
    img_w = w;
    img_h = h;
    if(w != h * int(count)){
        std::cerr << "Error: texture files must be N square textures packed horizontally." << std::endl;
        stbi_image_free(pixmap);
        return;
    }
    img = std::vector<uint32_t>(w * h);
    for(int j =0; j < h; ++j){
        for(int i = 0;i < w; ++i){
            uint8_t r = pixmap[(i + j * w) * 4 + 0];
            uint8_t g = pixmap[(i + j * w) * 4 + 1];
            uint8_t b = pixmap[(i + j * w) * 4 + 2];
            uint8_t a = pixmap[(i + j * w) * 4 + 3];
            img[i + j * w] = pack_color(r, g, b, a);
        }
    }
    stbi_image_free(pixmap);
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