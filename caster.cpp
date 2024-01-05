#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <sstream>
#include <iomanip>
#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"
uint32_t pack_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255)
{
    // 一个4 byte的无符号整数，每一个byte表示一个属性
    return (a << 24) + (b << 16) + (g << 8) + r;
}
void unpack_color(const uint32_t &color, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a)
{
    // 从给定的一个颜色，将属性抽出来
    r = (color >> 0) & 255;
    g = (color >> 8) & 255;
    b = (color >> 16) & 255;
    a = (color >> 24) & 255;
}
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
std::vector<uint32_t> texture_column(const std::vector<uint32_t> &img, const size_t texsize, const size_t tex_cnt, const size_t texid ,const size_t texcorrd, const size_t column_height){
    const size_t img_w = texsize * tex_cnt;
    const size_t img_h = texsize;
    std::cout << img.size() << std::endl;
    std::cout << img_w << std::endl;
    std::cout << img_h << std::endl;
    assert(img.size() == img_w * img_h && texcorrd < texsize && texid < tex_cnt);
    std::vector<uint32_t> column(column_height);
    for(size_t y = 0; y < column_height; ++y){
        // 依次前进，将质地图片对应的那一竖列每一个像素点的颜色复制到column数组
        size_t pix_x = texid * texsize + texcorrd;
        // 将这一列缩放到column_height的长度
        size_t pix_y = (y * texsize) / column_height;
        column[y] = img[pix_x + pix_y * img_w];
    }
    return column;
}
bool load_texture(const std::string filename, std::vector<uint32_t>& texture, size_t & text_size, size_t &text_cnt){
    int nchannels = -1, w, h;
    // 从图片加载墙面质地和怪物质地
    unsigned char *pixmap = stbi_load(filename.c_str(), &w, & h, &nchannels, 0);
    if(!pixmap){
        std::cerr << "Error: cannot load the textures" << std::endl;
        return false;
    }
    if(nchannels != 4){
        std::cerr << "Eoor: the texture must be 32 bit image" << std::endl;
        stbi_image_free(pixmap);
        return false;
    }
    text_cnt = w / h; // 有多少个质地
    text_size = w / text_cnt; // 一个质地的大小
    if(w != h * int(text_cnt)){
        std::cerr << "Error: texture files must be N square textures packed horizontally." << std::endl;
        stbi_image_free(pixmap);
        return false;
    }
    texture = std::vector<uint32_t>(w * h);
    for(int j =0; j < h; ++j){
        for(int i = 0;i < w; ++i){
            uint8_t r = pixmap[(i + j * w) * 4 + 0];
            uint8_t g = pixmap[(i + j * w) * 4 + 1];
            uint8_t b = pixmap[(i + j * w) * 4 + 2];
            uint8_t a = pixmap[(i + j * w) * 4 + 3];
            texture[i + j * w] = pack_color(r, g, b, a);
        }
    }
    stbi_image_free(pixmap);
    return true;
}

void draw_rectangle(std::vector<uint32_t> &img, const size_t img_w, const size_t img_h,
                    const size_t x, const size_t y, const size_t w, const size_t h,
                    const uint32_t color)
{

    assert(img.size() == img_w * img_h);
    for (size_t i = 0; i < w; ++i)
    {
        for (size_t j = 0; j < h; ++j)
        {
            size_t cx = x + i;
            size_t cy = y + j;
            if(cx >= img_w || cy >= img_h){
                continue;
            }
            img[cx + cy * img_w] = color;
        }
    }
}
int main()
{
    const size_t win_w = 1024;
    const size_t win_h = 512;
    std::vector<uint32_t> framebuffer(win_w * win_h, pack_color(255, 255, 255)); // 初始化为红色
    const size_t map_w = 16;                               // 地图宽度
    const size_t map_h = 16;                               // 地图高度
    const char map[] = "0000222222220000"
                       "1              0"
                       "1      11111   0"
                       "1     0        0"
                       "0     0  1110000"
                       "0     3        0"
                       "0   10000      0"
                       "0   3   11100  0"
                       "5   4   0      0"
                       "5   4   1  00000"
                       "0       1      0"
                       "2       1      0"
                       "0       0      0"
                       "0 0000000      0"
                       "0              0"
                       "0002222222200000";    // 游戏地图
    assert(sizeof(map) == map_h * map_w + 1); // 加上1因为有 '\0'
    float player_x = 3.456;                   // 玩家x轴坐标
    float player_y = 2.345;                   // 玩家y轴坐标
    float player_a = 1.523;                   // 玩家视线和x轴夹角
    const float sightAngle = M_PI / 3; // 视野角度，三分之一pi
    std::vector<uint32_t> walltext; // 墙壁质地
    size_t walltext_size;
    size_t walltext_cnt;
    if(!load_texture("./walltext.png", walltext, walltext_size, walltext_cnt)){
        std::cerr << "Failed to load wall textures" << std::endl;
        return -1;
    }
    for (size_t j = 0; j < win_h; j++)
    { // 初始化颜色变换
        for (size_t i = 0; i < win_w; i++)
        {
            uint8_t r = 255 * j / float(win_h);
            uint8_t g = 255 * i / float(win_w);
            uint8_t b = 0;
            framebuffer[i + j * win_w] = pack_color(r, g, b);
        }
    }
    // 一个地图块在图片里面是一个矩形像素块
    const size_t rect_w = win_w / (map_w*2);
    const size_t rect_h = win_h / map_h;
    // for(size_t frame = 0 ; frame < 360; ++frame){
    //     // 画出动画效果，总共有360帧
    //     std::stringstream ss;
    //     ss << std::setfill('0') << std::setw(5) << frame << ".ppm";
    //     player_a += 2*M_PI / 360;
    //     // 清空地图
    //     framebuffer = std::vector<uint32_t>(win_w* win_h, pack_color(255,255,255));
    //     for (size_t j = 0; j < map_h; j++){
    //     // 画地图
    //         for (size_t i = 0; i < map_w; i++)
    //         {
    //             if (map[i + j * map_w] == ' ')
    //             {
    //                 continue;
    //             }
    //             size_t rect_x = i * rect_w;
    //             size_t rect_y = j * rect_h;
    //             size_t icolor = map[i + j * map_w] - '0';
    //             assert(icolor < ncolors);
    //             draw_rectangle(framebuffer, win_w, win_h, rect_x, rect_y, rect_w, rect_h,
    //                         colors[icolor]);
    //         }
    //     }
    //     for(size_t i = 0; i < win_w / 2; ++i){
    //     // 画出视野范围, 这里遍历512次是为了画512条边，并且画3d的视野
    //     float angle = player_a - sightAngle / 2 + sightAngle * i / (float)(win_w / 2);
    //     // 画一条玩家的视线图
    //         for (float t = 0; t < 20; t += 0.01)
    //         {
    //             float cx = player_x + t * cos(angle);
    //             float cy = player_y + t * sin(angle);
    //             size_t ray_x = cx * rect_w;
    //             size_t ray_y = cy * rect_h;
    //             if(map[int(cx) + int(cy) * map_w] != ' '){
    //                 // 视线遇到了墙，要画出3d图像
    //                 size_t column_height = win_h / (t * cos(angle - player_a)); // 近大远小,使用cosine消除fish eye效应
    //                 size_t icolor = map[int(cx) + int(cy) * map_w] - '0';
    //                 assert(icolor < ncolors);
    //                 draw_rectangle(framebuffer, win_w, win_h, win_w/2 + i, win_h / 2 - column_height / 2,
    //                 1, column_height, colors[icolor]);
    //                 break;
    //             }
    //             framebuffer[ray_x + ray_y * win_w] = pack_color(160, 160, 160);
    //         }
    //     }
    //     // 采用ss处理字符流,标准化输入格式
    //     drop_ppm_image(ss.str(), framebuffer, win_w, win_h);
    // }
    for (size_t j=0; j<map_h; j++) { // draw the map
        for (size_t i=0; i<map_w; i++) {
            if (map[i+j*map_w]==' ') continue; // skip empty spaces
            size_t rect_x = i*rect_w;
            size_t rect_y = j*rect_h;
            size_t texid = map[i + j * map_w] - '0';
            assert(texid < walltext_cnt);
            draw_rectangle(framebuffer, win_w, win_h, rect_x, rect_y, rect_w, rect_h, walltext[texid * walltext_size]);
        }
    }
    for (size_t i=0; i<win_w/2; i++) { // draw the visibility cone AND the "3D" view
        float angle = player_a-sightAngle/2 + sightAngle*i/float(win_w/2);
        for (float t=0; t<20; t+=.01) {
            float cx = player_x + t*cos(angle);
            float cy = player_y + t*sin(angle);
            size_t pix_x = cx*rect_w;
            size_t pix_y = cy*rect_h;
            framebuffer[pix_x + pix_y*win_w] = pack_color(160, 160, 160); // this draws the visibility cone

            if (map[int(cx)+int(cy)*map_w]!=' ') { // our ray touches a wall, so draw the vertical column to create an illusion of 3D
               size_t texid = map[int(cx)+int(cy)*map_w] - '0';
                assert(texid<walltext_cnt);
                size_t column_height = win_h/(t*cos(angle-player_a));
                // 两个值包含了cx和cy的小数部分，-0.5到0.5
                float hitx = cx - floor(cx+.5);
                float hity = cy - floor(cy+.5);
                int x_texcoord = hitx * walltext_size; // 计算应该fetch哪一列
                if(std::abs(hity) > std::abs(hitx)){
                    // 遇到的是垂直的墙，相对与地图来说,应该用 hit_y来计算列数
                    x_texcoord = hity * walltext_size;
                }
                if(x_texcoord  < 0){
                    // 记得处理negative的情况
                    x_texcoord += walltext_size;
                }
                assert(x_texcoord >= 0 && x_texcoord < (int)walltext_size);
                std::vector<uint32_t>column = texture_column(walltext, walltext_size, walltext_cnt, texid, x_texcoord, column_height);
                pix_x = win_w / 2 + i; // 像素图上面的x坐标
                for(size_t j = 0; j < column_height; ++j){
                    pix_y = j + (win_h - column_height) / 2;
                    if(pix_y < 0 || pix_y > (int)win_h)
                    continue;
                    framebuffer[pix_x + pix_y * win_w] = column[j];
                }
                // draw_rectangle(framebuffer, win_w, win_h, win_w/2+i, win_h/2-column_height/2, 1, column_height, walltext[texid*walltext_size]);
                break;
            }
        }
    }
    drop_ppm_image("./out.ppm", framebuffer, win_w, win_h);
    return 0;
}