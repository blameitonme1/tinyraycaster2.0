#ifndef MAP_H
#define MAP_H
#include <cstdlib>
struct Map
{
    size_t w, h; // 地图的尺度
    Map();
    int get(const size_t i, const size_t j);
    bool is_empty(const size_t i, const size_t j);
};

#endif