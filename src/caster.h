#include "player.h"
#ifndef TINYRAYCASTER_H
#define TINYRAYCASTER_H
#include "monster.h"
#include "map.h"
#include "player.h"
#include "framebuffer.h"
#include "textures.h"
#include "utils.h"
struct GameState
{
    Map map;
    Player player;
    std::vector<Monster> monsters;
    Texture tex_walls;
    Texture tex_monst;
};

void render(FramBuffer &fb, GameState &gs);

#endif