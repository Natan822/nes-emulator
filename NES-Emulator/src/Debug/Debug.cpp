#include "Debug.h"
#include "../PPU/PPU.h"
#include "../NES/NES.h"
#include <array>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <thread>
#include <chrono>
#include <math.h>

namespace Debug
{
    bool quit = false;
    bool isInitialized = false;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    const int NAMETABLE_WIDTH = 256;
    const int NAMETABLE_HEIGHT = 240;

    NES *nes = NULL;

    int windowWidth = 0;
    int windowHeight = 0;

    void init(int width, int height, NES *targetNes)
    {
        nes = targetNes;

        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

        window = SDL_CreateWindow("NES Emulator - Debug Window", width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        if (window == NULL)
        {
            std::cerr << "ERROR (Debug): Failed to create SDL_Window\n";
            std::cerr << "SDL: " << SDL_GetError() << std::endl;
            return;
        }
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        windowWidth = width;
        windowHeight = height;

        renderer = SDL_CreateRenderer(window, NULL);
        if (renderer == NULL)
        {
            std::cerr << "ERROR (Debug): Failed to create SDL_Renderer\n";
            std::cerr << "SDL: " << SDL_GetError() << std::endl;
            return;
        }

        Nametables::init();

        SDL_ShowWindow(window);
        SDL_SetRenderVSync(renderer, 1);

        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard;
        io.IniFilename = nullptr;

        ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
        ImGui_ImplSDLRenderer3_Init(renderer);

        isInitialized = true;
    }

    void shutdown()
    {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        for (int i = 0; i < Nametables::nametableTextures.size(); i++)
        {
            SDL_DestroyTexture(Nametables::nametableTextures.at(i));
        }
    }

    void eventHandler(SDL_Event *event)
    {
        ImGui_ImplSDL3_ProcessEvent(event);
        if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
        {
            quit = true;
        }
    }

    namespace Renderer
    {
        std::array<ImVec2, 4> nametablesRectMin{};
        float nametableScale = 1;
        Tile selectedTile = {NULL, 0, 0};

        float getNametableScale()
        {
            return nametableScale;
        }

        void renderLoop()
        {
            while (!nes->isRunning)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(32));
            }

            while (!quit)
            {
                ImGui_ImplSDLRenderer3_NewFrame();
                ImGui_ImplSDL3_NewFrame();
                ImGui::NewFrame();

                SDL_GetWindowSize(window, &windowWidth, &windowHeight);

                ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
                ImGui::SetNextWindowPos(ImVec2(0, 0));

                ImGui::Begin("Debug Window", NULL, ImGuiWindowFlags_HorizontalScrollbar);

                if (ImGui::TreeNode("Nametables"))
                {
                    Nametables::updateAllNametables();
                    Nametables::updateNametablesTextures();
                    renderNametables();
                    ImGui::TreePop();
                }

                ImGui::End();
                ImGui::Render();

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);
                ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
                SDL_RenderPresent(renderer);
            }
            shutdown();
        }

        int scaleIndex = 1;
        const float allowedScales[] = {0.5f, 1.0f, 1.25f, 1.5f, 2.0f};
        const char *labels[] = {"0.5x", "1.0x", "1.25x", "1.5x", "2.0x"};
        void renderNametables()
        {
            if (ImGui::Combo("Scale", &scaleIndex, labels, IM_ARRAYSIZE(labels)))
            {
                nametableScale = allowedScales[scaleIndex];
            }

            float tileSize = 8 * getNametableScale(); // 8x8

            auto highlightTile = [&tileSize](ImVec2 tileAbsolutePos, ImVec2 nametableRectMin) -> void
            {
                ImVec2 relativeMousePos = {tileAbsolutePos.x - nametableRectMin.x, tileAbsolutePos.y - nametableRectMin.y};

                ImVec2 relativeTileRectMin = {static_cast<int>(relativeMousePos.x / tileSize) * tileSize, static_cast<int>(relativeMousePos.y / tileSize) * tileSize};
                ImVec2 tileRectMin = {relativeTileRectMin.x + nametableRectMin.x, relativeTileRectMin.y + nametableRectMin.y};
                ImVec2 tileRectMax = {tileRectMin.x + tileSize, tileRectMin.y + tileSize};

                ImDrawList *drawlist = ImGui::GetForegroundDrawList();
                ImU32 rectColor = IM_COL32(255, 255, 255, 255);
                drawlist->AddRect(tileRectMin, tileRectMax, rectColor, 0.0f, 0, getNametableScale());
            };

            auto handleHover = [&highlightTile](ImVec2 nametableRectMin) -> void
            {
                if (ImGui::IsItemHovered())
                {
                    highlightTile(ImGui::GetMousePos(), nametableRectMin);
                }
            };

            auto handleClick = [&highlightTile, &tileSize](int nametableIndex) -> void
            {
                if (ImGui::IsItemClicked())
                {
                    ImVec2 mousePos = ImGui::GetMousePos();
                    ImVec2 nametableRectMin = ImGui::GetItemRectMin();

                    ImVec2 relativeMousePos = {mousePos.x - nametableRectMin.x, mousePos.y - nametableRectMin.y};
                    ImVec2 relativeTilePos = {static_cast<int>(relativeMousePos.x / tileSize) * tileSize, static_cast<int>(relativeMousePos.y / tileSize) * tileSize};

                    isTileSelected = true;
                    selectedTile.indexInNametable = (relativeTilePos.x / tileSize) + (32 * static_cast<int>(relativeTilePos.y / tileSize));
                    selectedTile.nametableIndex = nametableIndex;
                    if (selectedTile.texture == NULL)
                    {
                        selectedTile.texture = SDL_CreateTexture(
                            renderer,
                            SDL_PIXELFORMAT_XRGB8888,
                            SDL_TEXTUREACCESS_STREAMING,
                            selectedTile.width,
                            selectedTile.height
                        );
                    }

                    if (selectedTile.frameBuffer == nullptr)
                    {
                        selectedTile.frameBuffer = (int *)malloc(selectedTile.width * selectedTile.height * sizeof(int));
                    }
                    updateSelectedTile(selectedTile.indexInNametable, selectedTile.nametableIndex);
                    SDL_UpdateTexture(selectedTile.texture, NULL, selectedTile.frameBuffer, sizeof(int) * selectedTile.width);
                    SDL_RenderTexture(renderer, selectedTile.texture, NULL, NULL);
                }
            };

            ImGui::BeginGroup();
            ImGui::Image((ImTextureID)Nametables::nametableTextures[0], ImVec2(NAMETABLE_WIDTH * getNametableScale(), NAMETABLE_HEIGHT * getNametableScale()));
            nametablesRectMin[0] = ImGui::GetItemRectMin();
            handleHover(nametablesRectMin[0]);
            handleClick(0);

            ImGui::SameLine();
            ImGui::Image((ImTextureID)Nametables::nametableTextures[1], ImVec2(NAMETABLE_WIDTH * getNametableScale(), NAMETABLE_HEIGHT * getNametableScale()));
            nametablesRectMin[1] = ImGui::GetItemRectMin();
            handleHover(nametablesRectMin[1]);
            handleClick(1);

            ImGui::Image((ImTextureID)Nametables::nametableTextures[2], ImVec2(NAMETABLE_WIDTH * getNametableScale(), NAMETABLE_HEIGHT * getNametableScale()));
            nametablesRectMin[2] = ImGui::GetItemRectMin();
            handleHover(nametablesRectMin[2]);
            handleClick(2);

            ImGui::SameLine();
            ImGui::Image((ImTextureID)Nametables::nametableTextures[3], ImVec2(NAMETABLE_WIDTH * getNametableScale(), NAMETABLE_HEIGHT * getNametableScale()));
            nametablesRectMin[3] = ImGui::GetItemRectMin();
            handleHover(nametablesRectMin[3]);
            handleClick(3);
            ImGui::EndGroup();

            if (isTileSelected)
            {
                ImVec2 absoluteTilePos = {
                    static_cast<float>(((selectedTile.indexInNametable % 32) * tileSize) + nametablesRectMin[selectedTile.nametableIndex].x),
                    static_cast<float>((selectedTile.indexInNametable / 32) * tileSize) + nametablesRectMin[selectedTile.nametableIndex].y
                };
                highlightTile(absoluteTilePos, nametablesRectMin[selectedTile.nametableIndex]);
                ImGui::SameLine();
                ImGui::BeginGroup();
                ImGui::Image((ImTextureID)selectedTile.texture, ImVec2(selectedTile.width * 5.0f, selectedTile.height * 5.0f));
                ImGui::Text("Selected tile: 0x%x", selectedTile.indexInNametable);
                ImGui::Text("Nametable: %d", selectedTile.nametableIndex);
                ImGui::Text("Nametable Address: 0x%x", selectedTile.address);

                ImGui::Text("Sprite Index: 0x%x", selectedTile.spriteIndex);
                ImGui::Text("Sprite Address: 0x%x", selectedTile.spriteAddress);
                
                ImGui::Text("Attribute Address: 0x%x", selectedTile.attrAddress);
                ImGui::Text("Attribute Byte: 0x%x", selectedTile.attrByte);
                
                ImGui::Text("Size: %dx%d", selectedTile.width, selectedTile.height);

                ImGui::Text("Palette Index: %d", selectedTile.paletteIndex);
                ImGui::EndGroup();
            }
        }

        void updateSelectedTile(int tileIndex, int nametableIndex)
        {
            const int BASE_NAMETABLE_ADDRESS = 0x2000 + (0x400 * nametableIndex);

            uint16_t tileAddress = BASE_NAMETABLE_ADDRESS + tileIndex;
            uint8_t tileIndexContent = ppuMemSnapshot.at(tileAddress);
            selectedTile.spriteIndex = tileIndexContent;
            selectedTile.address = tileAddress;

            uint16_t addressSprite = nes->ppu->backgroundPatternTableAddress + tileIndexContent * 16;
            selectedTile.spriteAddress = addressSprite;

            int tileY = selectedTile.indexInNametable / 32;
            int tileX = selectedTile.indexInNametable % 32;

            const int BASE_ATTR_TABLE_ADDRESS = BASE_NAMETABLE_ADDRESS + 960;
            uint16_t attrByteAddress = BASE_ATTR_TABLE_ADDRESS + (tileX / 4);
            attrByteAddress = attrByteAddress + ((tileY / 4) * 8);
            selectedTile.attrAddress = attrByteAddress;
            
            uint8_t attrByte = ppuMemSnapshot.at(attrByteAddress);
            selectedTile.attrByte = attrByte;

            int xQuadrant = (tileX % 4) * 8;
            int yQuadrant = (tileY % 4) * 8;
            int paletteIndex = nes->ppu->getPaletteIndex(xQuadrant, yQuadrant, attrByte);
            selectedTile.paletteIndex = paletteIndex;

            for (int y = 0; y < 8; y++)
            {
                uint8_t firstPlaneByte = ppuMemSnapshot.at(addressSprite + y);
                uint8_t secondPlaneByte = ppuMemSnapshot.at(addressSprite + y + 8);

                uint8_t byteMask = 0x80;
                for (int x = 0; x < 8; x++)
                {
                    uint8_t pixelBits = (((secondPlaneByte & byteMask) ? 1 : 0) << 1) | (firstPlaneByte & byteMask ? 1 : 0);
                    byteMask >>= 1;

                    uint8_t pixelValue;
                    if (pixelBits == 0)
                    {
                        pixelValue = ppuMemSnapshot.at(PALETTES_ADDRESS);
                    }
                    else
                    {
                        pixelValue = ppuMemSnapshot.at(PALETTES_ADDRESS + pixelBits + (4 * paletteIndex));
                    }

                    int pixelColor = nes->ppu->getPixelColor(pixelValue);
                    setSelectedTilePixel(x, y, pixelColor);
                }
            }
        }

        void setSelectedTilePixel(int x, int y, int pixelColor)
        {
            int pos = x + y * selectedTile.height;
            if (pos > selectedTile.height * selectedTile.width)
            {
                throw std::out_of_range("Pixel index out of range");
            }
            selectedTile.frameBuffer[pos] = pixelColor;
        }

    }
    namespace Nametables
    {
        std::array<SDL_Texture *, 4> nametableTextures{};
        std::array<std::array<uint32_t, NAMETABLE_WIDTH * NAMETABLE_HEIGHT>, 4> nametableFrameBuffers;

        void init()
        {
            for (int i = 0; i < 4; i++)
            {
                nametableTextures.at(i) = SDL_CreateTexture(
                    renderer,
                    SDL_PIXELFORMAT_XRGB8888,
                    SDL_TEXTUREACCESS_STREAMING,
                    NAMETABLE_WIDTH,
                    NAMETABLE_HEIGHT);

                if (nametableTextures.at(i) == NULL)
                {
                    std::cerr << "ERROR (Graphics): Failed to create SDL_Texture for nametable " << i + 1 << "\n";
                    std::cerr << "SDL: " << SDL_GetError() << std::endl;
                    return;
                }
            }
        }

        void updateAllNametables()
        {
            for (int i = 0; i < 4; i++)
            {
                updateNametable(i);
                SDL_UpdateTexture(nametableTextures.at(i), NULL, nametableFrameBuffers.at(i).data(), sizeof(uint32_t) * NAMETABLE_WIDTH);
            }
        }

        void updateNametablesTextures()
        {
            for (int i = 0; i < 4; i++)
            {
                SDL_RenderTexture(renderer, nametableTextures[i], NULL, NULL);
            }
        }

        void updateNametable(int nametableIndex)
        {
            ppuMemSnapshot = nes->ppu->getMemorySnapshot();

            const uint16_t BASE_NAMETABLE_ADDRESS = 0x2000 + (0x400 * nametableIndex);
            const uint16_t BASE_ATTR_TABLE_ADDRESS = BASE_NAMETABLE_ADDRESS + 960;

            uint16_t currentAttrByteAddress = 0;
            uint8_t attrByte = 0;
            for (int i = 0; i < 960; i++)
            {
                uint16_t tileAddress = BASE_NAMETABLE_ADDRESS + i;
                uint8_t tileIndex = ppuMemSnapshot.at(tileAddress);

                uint16_t addressSprite = nes->ppu->backgroundPatternTableAddress + tileIndex * 16;

                int tileX = i % 32;
                int tileY = i / 32;

                uint16_t attrByteAddress = BASE_ATTR_TABLE_ADDRESS + (tileX / 4);
                attrByteAddress = attrByteAddress + ((tileY / 4) * 8);

                // Reduce reads from PPU memory
                if (attrByteAddress != currentAttrByteAddress)
                {
                    attrByte = ppuMemSnapshot.at(attrByteAddress);
                    currentAttrByteAddress = attrByteAddress;
                }

                int xQuadrant = (tileX % 4) * 8;
                int yQuadrant = (tileY % 4) * 8;
                int paletteIndex = nes->ppu->getPaletteIndex(xQuadrant, yQuadrant, attrByte);

                for (int y = 0; y < 8; y++)
                {
                    uint8_t firstPlaneByte = ppuMemSnapshot.at(addressSprite + y);
                    uint8_t secondPlaneByte = ppuMemSnapshot.at(addressSprite + y + 8);

                    uint8_t byteMask = 0x80;
                    for (int x = 0; x < 8; x++)
                    {
                        uint8_t pixelBits = (((secondPlaneByte & byteMask) ? 1 : 0) << 1) | (firstPlaneByte & byteMask ? 1 : 0);
                        byteMask >>= 1;

                        uint8_t pixelValue;
                        if (pixelBits == 0)
                        {
                            pixelValue = ppuMemSnapshot.at(PALETTES_ADDRESS);
                        }
                        else
                        {
                            pixelValue = ppuMemSnapshot.at(PALETTES_ADDRESS + pixelBits + (4 * paletteIndex));
                        }

                        int pixelColor = nes->ppu->getPixelColor(pixelValue);
                        int pixelX = ((i * 8) % NAMETABLE_WIDTH) + x;
                        int pixelY = ((i / 32) * 8) + y;
                        setNametablePixel(nametableIndex, pixelX, pixelY, pixelColor);
                    }
                }
            }
        }

        void setNametablePixel(int nametableIndex, int x, int y, int pixelColor)
        {
            nametableFrameBuffers[nametableIndex].at((y * NAMETABLE_WIDTH) + x) = pixelColor;
        }
    };
}