#include "NametableViewer.h"
#include "../PPU/PPU.h"
#include <iostream>

void Tile::update(int _indexInNametable, int _nametableIndex, SDL_Renderer *renderer)
{
    indexInNametable = _indexInNametable;
    nametableIndex = _nametableIndex;

    SDL_DestroyTexture(texture);
    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_XRGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width,
		height);

    if (frameBuffer == nullptr)
    {
        frameBuffer = (int *)malloc(width * height * sizeof(int));
    }
}

NametableViewer::NametableViewer(SDL_Renderer *renderer, NES *nes)
    : m_renderer(renderer),
      m_nametableFrameBuffers(),
      m_nametableTextures(),
      m_ppuMemSnapshot(),
      m_nes(nes)
{
    for (int i = 0; i < 4; i++)
    {
        m_nametableTextures.at(i) = SDL_CreateTexture(
            m_renderer,
            SDL_PIXELFORMAT_XRGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            NAMETABLE_WIDTH,
            NAMETABLE_HEIGHT);

        if (m_nametableTextures.at(i) == NULL)
        {
            std::cerr << "ERROR (Graphics): Failed to create SDL_Texture for nametable " << i + 1 << "\n";
            std::cerr << "SDL: " << SDL_GetError() << std::endl;
            return;
        }
    }
}

NametableViewer::~NametableViewer()
{
    shutdown();
}

void NametableViewer::updateAllNametables()
{
    m_ppuMemSnapshot = m_nes->ppu->getMemorySnapshot();
    for (int i = 0; i < 4; i++)
    {
        updateNametable(i);
        SDL_UpdateTexture(m_nametableTextures.at(i), NULL, m_nametableFrameBuffers.at(i).data(), sizeof(uint32_t) * NAMETABLE_WIDTH);
    }
}

void NametableViewer::updateNametablesTextures()
{
    for (int i = 0; i < 4; i++)
    {
        SDL_RenderTexture(m_renderer, m_nametableTextures[i], NULL, NULL);
    }
}

void NametableViewer::updateNametable(int nametableIndex)
{
    const uint16_t BASE_NAMETABLE_ADDRESS = 0x2000 + (0x400 * nametableIndex);
    const uint16_t BASE_ATTR_TABLE_ADDRESS = BASE_NAMETABLE_ADDRESS + 960;

    uint16_t currentAttrByteAddress = 0;
    uint8_t attrByte = 0;
    for (int i = 0; i < 960; i++)
    {
        uint16_t tileAddress = BASE_NAMETABLE_ADDRESS + i;
        uint8_t tileIndex = m_ppuMemSnapshot.at(tileAddress);

        uint16_t addressSprite = m_nes->ppu->backgroundPatternTableAddress + tileIndex * 16;

        int tileX = i % 32;
        int tileY = i / 32;

        uint16_t attrByteAddress = BASE_ATTR_TABLE_ADDRESS + (tileX / 4);
        attrByteAddress = attrByteAddress + ((tileY / 4) * 8);

        // Reduce reads from PPU memory
        if (attrByteAddress != currentAttrByteAddress)
        {
            attrByte = m_ppuMemSnapshot.at(attrByteAddress);
            currentAttrByteAddress = attrByteAddress;
        }

        int xQuadrant = (tileX % 4) * 8;
        int yQuadrant = (tileY % 4) * 8;
        int paletteIndex = m_nes->ppu->getPaletteIndex(xQuadrant, yQuadrant, attrByte);

        for (int y = 0; y < 8; y++)
        {
            uint8_t firstPlaneByte = m_ppuMemSnapshot.at(addressSprite + y);
            uint8_t secondPlaneByte = m_ppuMemSnapshot.at(addressSprite + y + 8);

            uint8_t byteMask = 0x80;
            for (int x = 0; x < 8; x++)
            {
                uint8_t pixelBits = (((secondPlaneByte & byteMask) ? 1 : 0) << 1) | (firstPlaneByte & byteMask ? 1 : 0);
                byteMask >>= 1;

                uint8_t pixelValue;
                if (pixelBits == 0)
                {
                    pixelValue = m_ppuMemSnapshot.at(PALETTES_ADDRESS);
                }
                else
                {
                    pixelValue = m_ppuMemSnapshot.at(PALETTES_ADDRESS + pixelBits + (4 * paletteIndex));
                }

                int pixelColor = m_nes->ppu->getPixelColor(pixelValue);
                int pixelX = ((i * 8) % NAMETABLE_WIDTH) + x;
                int pixelY = ((i / 32) * 8) + y;
                setNametablePixel(nametableIndex, pixelX, pixelY, pixelColor);
            }
        }
    }
}

void NametableViewer::setNametablePixel(int nametableIndex, int x, int y, int pixelColor)
{
    m_nametableFrameBuffers[nametableIndex].at((y * NAMETABLE_WIDTH) + x) = pixelColor;
}

void NametableViewer::updateSelectedTilePixels()
{
    const int BASE_NAMETABLE_ADDRESS = 0x2000 + (0x400 * m_selectedTile.nametableIndex);

    uint16_t tileAddress = BASE_NAMETABLE_ADDRESS + m_selectedTile.indexInNametable;
    uint8_t tileIndexContent = m_ppuMemSnapshot.at(tileAddress);
    m_selectedTile.spriteIndex = tileIndexContent;
    m_selectedTile.address = tileAddress;

    uint16_t addressSprite = m_nes->ppu->backgroundPatternTableAddress + tileIndexContent * 16;
    m_selectedTile.spriteAddress = addressSprite;

    int tileY = m_selectedTile.indexInNametable / 32;
    int tileX = m_selectedTile.indexInNametable % 32;

    const int BASE_ATTR_TABLE_ADDRESS = BASE_NAMETABLE_ADDRESS + 960;
    uint16_t attrByteAddress = BASE_ATTR_TABLE_ADDRESS + (tileX / 4);
    attrByteAddress = attrByteAddress + ((tileY / 4) * 8);
    m_selectedTile.attrAddress = attrByteAddress;

    uint8_t attrByte = m_ppuMemSnapshot.at(attrByteAddress);
    m_selectedTile.attrByte = attrByte;

    int xQuadrant = (tileX % 4) * 8;
    int yQuadrant = (tileY % 4) * 8;
    int paletteIndex = m_nes->ppu->getPaletteIndex(xQuadrant, yQuadrant, attrByte);
    m_selectedTile.paletteIndex = paletteIndex;

    for (int y = 0; y < 8; y++)
    {
        uint8_t firstPlaneByte = m_ppuMemSnapshot.at(addressSprite + y);
        uint8_t secondPlaneByte = m_ppuMemSnapshot.at(addressSprite + y + 8);

        uint8_t byteMask = 0x80;
        for (int x = 0; x < 8; x++)
        {
            uint8_t pixelBits = (((secondPlaneByte & byteMask) ? 1 : 0) << 1) | (firstPlaneByte & byteMask ? 1 : 0);
            byteMask >>= 1;

            uint8_t pixelValue;
            if (pixelBits == 0)
            {
                pixelValue = m_ppuMemSnapshot.at(PALETTES_ADDRESS);
            }
            else
            {
                pixelValue = m_ppuMemSnapshot.at(PALETTES_ADDRESS + pixelBits + (4 * paletteIndex));
            }

            int pixelColor = m_nes->ppu->getPixelColor(pixelValue);
            setSelectedTilePixel(x, y, pixelColor);
        }
    }
}

void NametableViewer::setSelectedTilePixel(int x, int y, int pixelColor)
{
    int pos = x + y * m_selectedTile.height;
    if (pos > m_selectedTile.height * m_selectedTile.width)
    {
        throw std::out_of_range("Pixel index out of range");
    }
    m_selectedTile.frameBuffer[pos] = pixelColor;
}

Tile &NametableViewer::getSelectedTile()
{
    return m_selectedTile;
}

void NametableViewer::setSelectedTile(Tile &tile)
{
    m_selectedTile = tile;
}

std::array<SDL_Texture *, 4> &NametableViewer::getNametableTextures()
{
    return m_nametableTextures;
}

void NametableViewer::shutdown()
{
    for (int i = 0; i < m_nametableTextures.size(); i++)
    {
        SDL_DestroyTexture(m_nametableTextures.at(i));
    }
}