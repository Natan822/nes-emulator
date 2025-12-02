#pragma once

#include <SDL3/SDL.h>
#include <atomic>
#include <array>
#include <cstdint>

class NES;
namespace Debug
{
    extern SDL_Window *window;

    // Init Debug Window
    void init(int width, int height, NES* targetNes);
    void renderLoop();
    void shutdown();

    extern bool quit;

    namespace Nametables
    {
        extern std::atomic<bool> shouldUpdateNametables;
        extern std::array<uint8_t, 0x4000> ppuMem;
        extern std::array<SDL_Texture *, 4> nametableTextures;

        void init();

        void updatePpuMem(std::array<uint8_t, 0x4000> ppuMem);
        void updateAllNametables();
        void updateLoop();
        void updateNametable(int nametableIndex);
        void updateNametablesTextures();

        void setNametablePixel(int nametableIndex, int x, int y, int pixelColor);
    };
};