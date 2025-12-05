#pragma once

#include <SDL3/SDL.h>
#include <atomic>
#include <array>
#include <cstdint>

class NES;
namespace Debug
{
    extern SDL_Window *window;
    extern bool isInitialized;

    // Init Debug Window
    void init(int width, int height, NES *targetNes);
    void shutdown();
    void eventHandler(SDL_Event *event);

    extern bool quit;

    namespace Renderer
    {
        extern float nametableScale;

        void renderLoop();
        void renderNametables();
    }

    namespace Nametables
    {
        extern std::array<SDL_Texture *, 4> nametableTextures;

        void init();

        void updateAllNametables();
        void updateLoop();
        void updateNametable(int nametableIndex);
        void updateNametablesTextures();

        void setNametablePixel(int nametableIndex, int x, int y, int pixelColor);
    };
};