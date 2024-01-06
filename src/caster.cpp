#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdint>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <Monster.h>
#include "map.h"
#include "player.h"
#include "framebuffer.h"
#include "textures.h"
#include "utils.h"
/// @brief 返回对应质地里对应的列数
/// @param x 
/// @param y 
/// @param texWall 
/// @return 
int wall_x_texcoord(const float x, const float y, Texture &texWall){
    float hitx = x - floor(x+.5);
    float hity = y - floor(y+.5); // 计算坐标的小数部分，取值范围在-0.5 到 0.5
    int texcoord = hitx * texWall.size;
    if(std::abs(hity) > std::abs(hitx)){
        // 处理的是数值的墙，要使用hity
        texcoord = hity * texWall.size;
    }
    if(texcoord < 0){
        texcoord += texWall.size;
    }
    assert(texcoord >= 0 && texcoord < (int)texWall.size);
    return texcoord;
}
/// @brief 在地图显示怪物坐标
/// @param monster 
/// @param fb 
/// @param map 
void show_monster_inmap(Monster &monster, FramBuffer &fb, Map &map){
    const size_t rect_w = fb.w / (map.w * 2);
    const size_t rect_h = fb.h / map.h;
    // 画一个6像素宽高的怪物显示地图上，-3是为了取得左上角的坐标
    fb.draw_rectangle(monster.x * rect_w - 3, monster.y * rect_h - 3, 6, 6, pack_color(255, 0, 0));
}
/// @brief 根据invariance画出图像
/// @param fb 
/// @param map 
/// @param player 
/// @param textures 
void render(FramBuffer &fb, Map &map, Player &player, Texture &textures, std::vector<Monster>& monsters, Texture &tex_monst){
    fb.clear(pack_color(255, 255, 255)); // 清空图片
    const size_t rect_w = fb.w / (map.w * 2);
    const size_t rect_h = fb.h / map.h; // 一个地图方块对应的像素图里面的宽和高
    // 先画左半的地图
    for(size_t j = 0; j < map.h; ++j){
        for(size_t i = 0; i < map.w; ++i){
            if(map.is_empty(i, j)) continue; // 没有遇到墙就继续遍历
            size_t rect_x = i * rect_w;
            size_t rect_y = j * rect_h;
            size_t texid = map.get(i, j);
            assert(texid < textures.count);
            // 选择texid的质地的左上方的像素的颜色
            fb.draw_rectangle(rect_x, rect_y, rect_w, rect_h, textures.get(0, 0, texid));
        }
    }
    // 开始画视线图, i 遍历每一列
    for(size_t i = 0; i < fb.w / 2; ++i){
        // 计算角度
        float angle = player.a - player.sightAngle/2 + player.sightAngle * i / float(fb.w / 2);
        for(float t = 0; t < 20; t += 0.01){
            float x = player.x + t * cos(angle);
            float y = player.y + t * sin(angle);
            // 画出视线图
            fb.set_pixel(x * rect_w, y * rect_h, pack_color(160, 160, 160));
            if(map.is_empty(x, y)) continue;
            size_t texid = map.get(x, y);
            assert(texid < textures.count);
            // 使用几何关系消除鱼眼的视觉效应
            float dist = t * cos(angle - player.a);
            size_t colum_height = fb.h / dist;
            int x_texcoord = wall_x_texcoord(x, y, textures);
            std::vector<uint32_t> column = textures.get_scaled_colum(texid, x_texcoord, colum_height);
            // 接下来在像素图将得到的column的颜色信息显示在图片里面
            int pix_x = i + fb.w / 2;
            for(size_t j = 0; j < colum_height; ++j){
                int pix_y = j + (fb.h - colum_height) / 2;
                if(pix_y >= 0 && pix_y < (int)fb.h){
                    fb.set_pixel(pix_x, pix_y, column[j]);
                }
            }
            break; // 因为遇到了墙，此时不能继续前进
        }
    }
    for(size_t i = 0; i < monsters.size(); ++i){
        show_monster_inmap(monsters[i], fb, map);
    }
}

int main(){
    FramBuffer fb{1024, 512, std::vector<uint32_t>(1024*512, pack_color(255, 255, 255))};
    Player player{3.456, 2.345, 1.523, M_PI / 3.};
    Map map;
    Texture tex_walls("./walltext.png");
    Texture tex_monsts("./monsters.png");
    if(!tex_walls.count || !tex_monsts.count){
        std::cerr << "Failed to load textures." << std::endl;
        return -1;
    }
    std::vector<Monster> monsters{ {1.834, 8.765, 0}, {5.323, 5.365, 1}, {4.123, 10.265, 1} };
    for(size_t frame = 0; frame < 360; ++frame){
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(5) << frame << ".ppm";
        player.a += 2 * M_PI / 360;
        render(fb, map, player, tex_walls, monsters, tex_monsts);
        drop_ppm_image(ss.str(), fb.img, fb.w, fb.h);
    }
    return 0;
}