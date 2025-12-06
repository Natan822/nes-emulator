#pragma once

#include <SDL3/SDL.h>
#include <imgui.h>
#include <atomic>
#include <array>
#include <cstdint>

class NES;
namespace Debug
{
    extern SDL_Window *window;
    extern bool isInitialized;
    inline std::array<uint8_t, 0x4000> ppuMemSnapshot{};

    // Init Debug Window
    void init(int width, int height, NES *targetNes);
    void shutdown();
    void eventHandler(SDL_Event *event);

    extern bool quit;

    namespace Renderer
    {
        extern float nametableScale;
        extern std::array<ImVec2, 4> nametablesRectMin;
        inline bool isTileSelected = false;

        struct Tile
        {
            SDL_Texture* texture;
            int nametableIndex;
            int tileIndex;
            int paletteIndex = 0;
            int* frameBuffer = nullptr;
            int width = 8;
            int height = 8;
        };
        extern Tile selectedTile;

        void setSelectedTilePixel(int x, int y, int pixelColor);

        float getNametableScale();
        void renderLoop();
        void renderNametables();
        void updateSelectedTile(int tileIndex, int nametableIndex);
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