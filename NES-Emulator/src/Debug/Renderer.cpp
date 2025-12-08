#include "Renderer.h"
#include "../App.h"
#include "Debugger.h"
#include "NametableViewer.h"
#include "../NES/NES.h"
#include <thread>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

Renderer::Renderer(NES* nes, SDL_Window* window, SDL_Renderer* renderer)
    : m_nametableScale(1.0f),
      m_nametablesRectMin(),
      m_nes(nes),
      m_window(window),
      m_renderer(renderer),
      m_isTileSelected(false)
{
    m_nametableViewer = std::make_unique<NametableViewer>(m_renderer, m_nes);
}

Renderer::~Renderer()
{
    shutdown();
}

float Renderer::getNametableScale()
{
    return m_nametableScale;
}

void Renderer::renderLoop()
{
    while (!m_nes->isRunning)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(32));
    }

    while (!Debugger::quit)
    {
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        SDL_GetWindowSize(m_window, &m_windowWidth, &m_windowHeight);

        ImGui::SetNextWindowSize(ImVec2(m_windowWidth, m_windowHeight));
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        ImGui::Begin("Debug Window", NULL, ImGuiWindowFlags_HorizontalScrollbar);

        if (ImGui::TreeNode("Nametables"))
        {
            m_nametableViewer->updateAllNametables();
            m_nametableViewer->updateNametablesTextures();
            renderNametables();
            ImGui::TreePop();
        }

        ImGui::End();
        ImGui::Render();

        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
        SDL_RenderClear(m_renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_renderer);
        SDL_RenderPresent(m_renderer);
    }
    App::debugger->shutdown();
}

int scaleIndex = 1;
const float allowedScales[] = {0.5f, 1.0f, 1.25f, 1.5f, 2.0f};
const char *labels[] = {"0.5x", "1.0x", "1.25x", "1.5x", "2.0x"};
void Renderer::renderNametables()
{
    if (ImGui::Combo("Scale", &scaleIndex, labels, IM_ARRAYSIZE(labels)))
    {
        m_nametableScale = allowedScales[scaleIndex];
    }

    float tileSize = 8 * getNametableScale(); // 8x8


    std::array<SDL_Texture *, 4> nametableTextures = m_nametableViewer->getNametableTextures();
    ImGui::BeginGroup();
    ImGui::Image((ImTextureID)nametableTextures[0], ImVec2(NametableViewer::NAMETABLE_WIDTH * getNametableScale(), NametableViewer::NAMETABLE_HEIGHT * getNametableScale()));
    m_nametablesRectMin[0] = ImGui::GetItemRectMin();
    handleHoverNT(m_nametablesRectMin[0], tileSize);
    handleClickNT(0, tileSize);

    ImGui::SameLine();
    ImGui::Image((ImTextureID)nametableTextures[1], ImVec2(NametableViewer::NAMETABLE_WIDTH * getNametableScale(), NametableViewer::NAMETABLE_HEIGHT * getNametableScale()));
    m_nametablesRectMin[1] = ImGui::GetItemRectMin();
    handleHoverNT(m_nametablesRectMin[1], tileSize);
    handleClickNT(1, tileSize);

    ImGui::Image((ImTextureID)nametableTextures[2], ImVec2(NametableViewer::NAMETABLE_WIDTH * getNametableScale(), NametableViewer::NAMETABLE_HEIGHT * getNametableScale()));
    m_nametablesRectMin[2] = ImGui::GetItemRectMin();
    handleHoverNT(m_nametablesRectMin[2], tileSize);
    handleClickNT(2, tileSize);

    ImGui::SameLine();
    ImGui::Image((ImTextureID)nametableTextures[3], ImVec2(NametableViewer::NAMETABLE_WIDTH * getNametableScale(), NametableViewer::NAMETABLE_HEIGHT * getNametableScale()));
    m_nametablesRectMin[3] = ImGui::GetItemRectMin();
    handleHoverNT(m_nametablesRectMin[3], tileSize);
    handleClickNT(3, tileSize);
    ImGui::EndGroup();

    if (m_isTileSelected)
    {
        renderSelectedTileNT(tileSize);
    }
}

void Renderer::highlightTileNT(ImVec2 tileAbsolutePos, ImVec2 nametableRectMin, float tileSize)
{
    ImVec2 relativeMousePos = {tileAbsolutePos.x - nametableRectMin.x, tileAbsolutePos.y - nametableRectMin.y};

    ImVec2 relativeTileRectMin = {static_cast<int>(relativeMousePos.x / tileSize) * tileSize, static_cast<int>(relativeMousePos.y / tileSize) * tileSize};
    ImVec2 tileRectMin = {relativeTileRectMin.x + nametableRectMin.x, relativeTileRectMin.y + nametableRectMin.y};
    ImVec2 tileRectMax = {tileRectMin.x + tileSize, tileRectMin.y + tileSize};

    ImDrawList *drawlist = ImGui::GetForegroundDrawList();
    ImU32 rectColor = IM_COL32(255, 255, 255, 255);
    drawlist->AddRect(tileRectMin, tileRectMax, rectColor, 0.0f, 0, getNametableScale());
}

void Renderer::handleHoverNT(ImVec2 nametableRectMin, float tileSize)
{
    if (ImGui::IsItemHovered())
    {
        highlightTileNT(ImGui::GetMousePos(), nametableRectMin, tileSize);
    }
}

void Renderer::handleClickNT(int nametableIndex, float tileSize)
{
    if (ImGui::IsItemClicked())
    {
        ImVec2 mousePos = ImGui::GetMousePos();
        ImVec2 nametableRectMin = ImGui::GetItemRectMin();

        ImVec2 relativeMousePos = {mousePos.x - nametableRectMin.x, mousePos.y - nametableRectMin.y};
        ImVec2 relativeTilePos = {static_cast<int>(relativeMousePos.x / tileSize) * tileSize, static_cast<int>(relativeMousePos.y / tileSize) * tileSize};

        m_isTileSelected = true;
        int indexInNametable = (relativeTilePos.x / tileSize) + (32 * static_cast<int>(relativeTilePos.y / tileSize));

        Tile& selectedTile = m_nametableViewer->getSelectedTile();
        selectedTile.update(indexInNametable, nametableIndex, m_renderer);

        m_nametableViewer->setSelectedTile(selectedTile);
        m_nametableViewer->updateSelectedTilePixels();

        SDL_UpdateTexture(m_nametableViewer->getSelectedTile().texture, NULL, m_nametableViewer->getSelectedTile().frameBuffer, sizeof(int) * m_nametableViewer->getSelectedTile().width);
        SDL_RenderTexture(m_renderer, m_nametableViewer->getSelectedTile().texture, NULL, NULL);
    }
}

void Renderer::renderSelectedTileNT(float tileSize)
{
    Tile& selectedTile = m_nametableViewer->getSelectedTile();

    ImVec2 absoluteTilePos = {
        static_cast<float>(((selectedTile.indexInNametable % 32) * tileSize) + m_nametablesRectMin[selectedTile.nametableIndex].x),
        static_cast<float>((selectedTile.indexInNametable / 32) * tileSize) + m_nametablesRectMin[selectedTile.nametableIndex].y};
    highlightTileNT(absoluteTilePos, m_nametablesRectMin[selectedTile.nametableIndex], tileSize);
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

void Renderer::shutdown()
{
    m_nametableViewer->shutdown();
}