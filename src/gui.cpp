#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <chrono>
#include <thread>
#include "map.h"
#include "utils.h"
#include "player.h"
#include "monster.h"
#include "framebuffer.h"
#include "textures.h"
#include "caster.h"
int main()
{
    FramBuffer fb{1024, 512, std::vector<uint32_t>(1024 * 512, pack_color(255, 255, 255))};
    GameState gs{Map(),                                  // game map
                 {3.456, 2.345, 1.523, M_PI / 3., 0, 0}, // player
                 {{3.523, 3.812, 2},                     // monsters lists
                  {1.834, 8.765, 0},
                  {5.323, 5.365, 1},
                  {14.32, 13.36, 3},
                  {4.123, 10.76, 1}},
                 Texture("../walltext.bmp", SDL_PIXELFORMAT_ABGR8888),  // textures for the walls
                 Texture("../monsters.bmp", SDL_PIXELFORMAT_ABGR8888)}; // textures for the monsters
    if (!gs.tex_walls.count || !gs.tex_monst.count)
    {
        std::cerr << "Failed to load textures" << std::endl;
        return -1;
    }
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "Couldn't initialize SDL:  " << SDL_GetError() << std::endl;
        return -1;
    }
    if (SDL_CreateWindowAndRenderer(fb.w, fb.h, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS, &window, &renderer))
    {
        std::cerr << "Couldn't create window and renderer:  " << SDL_GetError() << std::endl;
        return -1;
    }
    // 无法成功创建质地对象
    SDL_Texture *framebuffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, fb.w, fb.h);
    if (!framebuffer_texture)
    {
        std::cerr << "Failed to create framebuffer texture : " << SDL_GetError() << std::endl;
        return -1;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    while (1)
    {
        std::cout << "again" << std::endl;
        // 捕获gui的事件
        {
            // 至少20毫秒渲染一次画面
            auto t2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> fp_ms = t2 - t1;
            if (fp_ms.count() < 20)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(3));
                continue;
            }
            t1 = t2;
        }
        {
            // 更新玩家状态
            SDL_Event event;
            if (SDL_PollEvent(&event))
            {
                // 检查用户多久退出
                if (SDL_QUIT == event.type || (SDL_KEYDOWN == event.type && SDLK_ESCAPE == event.key.keysym.sym))
                    break;
                if (SDL_KEYUP == event.type)
                {
                    // 转或者走
                    if ('a' == event.key.keysym.sym || 'd' == event.key.keysym.sym)
                        gs.player.turn = 0;
                    if ('w' == event.key.keysym.sym || 's' == event.key.keysym.sym)
                        gs.player.walk = 0;
                }
                if (SDL_KEYDOWN == event.type)
                {
                    if ('a' == event.key.keysym.sym)
                        gs.player.turn = -1;
                    if ('d' == event.key.keysym.sym)
                        gs.player.turn = 1;
                    if ('w' == event.key.keysym.sym)
                        gs.player.walk = 1;
                    if ('s' == event.key.keysym.sym)
                        gs.player.walk = -1;
                }
            }
        }
        {
            // 更新玩家的位置s
            gs.player.a += float(gs.player.turn) * .05;
            float nx = gs.player.x + gs.player.walk * cos(gs.player.a) * .05;
            float ny = gs.player.y + gs.player.walk * sin(gs.player.a) * .05;

            if (int(nx) >= 0 && int(nx) < int(gs.map.w) && int(ny) >= 0 && int(ny) < int(gs.map.h))
            {
                if (gs.map.is_empty(nx, gs.player.y))
                    gs.player.x = nx;
                if (gs.map.is_empty(gs.player.x, ny))
                    gs.player.y = ny;
            }
        }

        render(fb, gs); // 渲染画面，framebuffer而不是实际gui画面

        { // 将渲染结果更新到纹理
            SDL_UpdateTexture(framebuffer_texture, NULL, reinterpret_cast<void *>(fb.img.data()), fb.w * 4);

            // 清除渲染器
            SDL_RenderClear(renderer);

            // 将纹理内容绘制到窗口
            SDL_RenderCopy(renderer, framebuffer_texture, NULL, NULL);

            // 显示渲染的结果
            SDL_RenderPresent(renderer);
        }
    }
    SDL_DestroyTexture(framebuffer_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}