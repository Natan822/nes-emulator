#include <array>
#include <SDL3/SDL.h>
#include <cstdint>
#include "../NES/NES.h"

struct Tile
{
    SDL_Texture *texture;
    int nametableIndex;

    int indexInNametable;
    int address;

    int spriteIndex;
    int spriteAddress;

    int attrAddress;
    int attrByte;

    int paletteIndex = 0;
    int *frameBuffer = nullptr;
    int width = 8;
    int height = 8;

    void update(int _indexInNametable, int _nametableIndex, SDL_Renderer *renderer);
};

class NametableViewer
{
public:
    static const int NAMETABLE_WIDTH = 256;
    static const int NAMETABLE_HEIGHT = 240;

    NametableViewer(SDL_Renderer* renderer, NES* nes);
    ~NametableViewer();

    void updateAllNametables();
    void updateNametablesTextures();

    void updateNametable(int nametableIndex);
    void setNametablePixel(int nametableIndex, int x, int y, int pixelColor);

    void updateSelectedTilePixels();
    void setSelectedTilePixel(int x, int y, int pixelColor);

    Tile &getSelectedTile();
    void setSelectedTile(Tile& tile);

    std::array<SDL_Texture *, 4> &getNametableTextures();

    void shutdown();

private:
    NES* m_nes;
    SDL_Renderer* m_renderer;
    std::array<uint8_t, 0x4000> m_ppuMemSnapshot;
    Tile m_selectedTile;

    std::array<SDL_Texture *, 4> m_nametableTextures;
    std::array<std::array<uint32_t, NAMETABLE_WIDTH * NAMETABLE_HEIGHT>, 4> m_nametableFrameBuffers;
};