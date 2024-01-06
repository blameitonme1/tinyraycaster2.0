#ifndef PLAYER_H
#define PLAYER_H
struct Player
{
    float x, y; // 玩家位置
    float a; // 视线和x轴的夹角
    float sightAngle; // 视野角度
    int turn, walk; // 转向和移动方向
};

#endif