#include <array>
#include <SDL3/SDL.h>
#include <imgui.h>
#include <memory>

class NES;
class NametableViewer;


class Renderer
{
public:
    Renderer(NES* nes, SDL_Window* window, SDL_Renderer* renderer);
    ~Renderer();

    void renderLoop();
    void shutdown();

private:
    int m_windowWidth;
    int m_windowHeight;

    bool m_isTileSelected;

    SDL_Window* m_window;
    SDL_Renderer* m_renderer;

    std::unique_ptr<NametableViewer> m_nametableViewer;

    NES* m_nes;
    std::array<ImVec2, 4> m_nametablesRectMin;
    float m_nametableScale;

    float getNametableScale();
    void renderNametables();
    void setSelectedTilePixel(int x, int y, int pixelColor);

    // Nametables events
    void handleHoverNT(ImVec2 nametableRectMin, float tileSize);
    void handleClickNT(int nametableIndex, float tileSize);
    void highlightTileNT(ImVec2 tileAbsolutePos, ImVec2 nametableRectMin, float tileSize);
    void renderSelectedTileNT(float tileSize);
};