#include <memory>
#include <SDL3/SDL.h>

class NES;
class NametableViewer;
class Renderer;

class Debugger
{
public:
    static bool quit;
    static bool isInitialized;

    Debugger(int width, int height, NES* nes);
    ~Debugger();

    void startRenderLoop();
    void shutdown();

    static void eventHandler(SDL_Event *event);
    
private:
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;

    std::unique_ptr<Renderer> m_debuggerRenderer;

    NES* m_nes;

    int m_windowWidth;
    int m_windowHeight;

};