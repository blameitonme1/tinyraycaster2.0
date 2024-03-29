#include <cstdlib>
#include <cassert>
#include "map.h"
// 定义地图以及每个墙壁方块对应的质地标号
static const char map[] = "0000222222220000"\
                          "1              0"\
                          "1      11111   0"\
                          "1     0        0"\
                          "0     0  1110000"\
                          "0     3        0"\
                          "0   10000      0"\
                          "0   3   11100  0"\
                          "5   4   0      0"\
                          "5   4   1  00000"\
                          "0       1      0"\
                          "2       1      0"\
                          "0       0      0"\
                          "0 0000000      0"\
                          "0              0"\
                          "0002222222200000";

Map::Map() : w(16), h(16){
    assert(sizeof(map) == w * h + 1); // 检查地图的大小是否正确，+1因为map结尾有null
}
/// @brief 返回地图对应 (i , j) 坐标的质地的标号
/// @param i 
/// @param j 
/// @return 
int Map::get(const size_t i, const size_t j){
    assert(i < w && j < h && sizeof(map) == w * h + 1);
    return map[i + j * w] - '0';
}
/// @brief 判断地图对应 (i, j) 坐标是否为墙
/// @param i 
/// @param j 
/// @return 
bool Map::is_empty(const size_t i , const size_t j){
    assert(i < w && j < h && sizeof(map) == w * h + 1);
    return map[i + j * w] == ' ';
}
