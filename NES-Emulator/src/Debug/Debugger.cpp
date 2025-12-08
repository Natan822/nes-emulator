#include "Debugger.h"
#include "NametableViewer.h"
#include "Renderer.h"
#include <array>
#include <SDL3/SDL.h>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

bool Debugger::quit = false;
bool Debugger::isInitialized = false;

Debugger::Debugger(int width, int height, NES* nes)
    : m_windowWidth(width),
      m_windowHeight(height),
      m_nes(nes)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    m_window = SDL_CreateWindow("NES Emulator - Debug Window", width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (m_window == NULL)
    {
        std::cerr << "ERROR (Debug): Failed to create SDL_Window\n";
        std::cerr << "SDL: " << SDL_GetError() << std::endl;
        return;
    }
    SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    m_renderer = SDL_CreateRenderer(m_window, NULL);
    if (m_renderer == NULL)
    {
        std::cerr << "ERROR (Debug): Failed to create SDL_Renderer\n";
        std::cerr << "SDL: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_ShowWindow(m_window);
    SDL_SetRenderVSync(m_renderer, 1);
    
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr;
    
    ImGui_ImplSDL3_InitForSDLRenderer(m_window, m_renderer);
    ImGui_ImplSDLRenderer3_Init(m_renderer);
    
    m_debuggerRenderer = std::make_unique<Renderer>(m_nes, m_window, m_renderer);

    isInitialized = true;
}

Debugger::~Debugger()
{
    shutdown();
}

void Debugger::startRenderLoop()
{
    m_debuggerRenderer->renderLoop();
}

void Debugger::shutdown()
{
    m_debuggerRenderer->shutdown();
    
    SDL_DestroyWindow(m_window);
    SDL_DestroyRenderer(m_renderer);
}

void Debugger::eventHandler(SDL_Event *event)
{
    ImGui_ImplSDL3_ProcessEvent(event);
    if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
    {
        quit = true;
    }
}