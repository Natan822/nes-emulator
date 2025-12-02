#include "../Controller/Controller.h"
#include "../NES/NES.h"
#include "../Debug/Debug.h"
#include <SDL3/SDL.h>
#include "Input.h"
#include "../Graphics/Graphics.h"

namespace Input {
	void inputProcessing(Controller* controller) {
		SDL_Event event;

		while (SDL_PollEvent(&event)) 
		{
			switch (event.type)
			{
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				// Close emulator window
				if (event.window.windowID == SDL_GetWindowID(Graphics::window)) {
					NES::isRunning = false;
				}
				// Close debug window
				Debug::quit = true;
				break;
			case SDL_EVENT_KEY_DOWN:
				handleKeyDown(controller, event);
				break;
			case SDL_EVENT_KEY_UP:
				handleKeyUp(controller, event);
				break;
			}
		}
	}

	void handleKeyDown(Controller* controller, SDL_Event event) {
		switch (event.key.key)
		{
		case SDLK_ESCAPE:
			NES::isRunning = false;
			break;
		case SDLK_P:
			pause();
			break;
		case SDLK_W:
			controller->buttonPress('U');
			break;
		case SDLK_S:
			controller->buttonPress('D');
			break;
		case SDLK_A:
			controller->buttonPress('L');
			break;
		case SDLK_D:
			controller->buttonPress('R');
			break;
		case SDLK_O:
			controller->buttonPress('S');
			break;
		case SDLK_I:
			controller->buttonPress('E');
			break;
		case SDLK_K:
			controller->buttonPress('B');
			break;
		case SDLK_L:
			controller->buttonPress('A');
			break;
		}
	}

	void handleKeyUp(Controller* controller, SDL_Event event) {
		switch (event.key.key)
		{
		case SDLK_W:
			controller->buttonRelease('U');
			break;
		case SDLK_S:
			controller->buttonRelease('D');
			break;
		case SDLK_A:
			controller->buttonRelease('L');
			break;
		case SDLK_D:
			controller->buttonRelease('R');
			break;
		case SDLK_O:
			controller->buttonRelease('S');
			break;
		case SDLK_I:
			controller->buttonRelease('E');
			break;
		case SDLK_K:
			controller->buttonRelease('B');
			break;
		case SDLK_L:
			controller->buttonRelease('A');
			break;
		}
	}

	void pause() {
		bool isPaused = true;
		SDL_Event event;
		while (isPaused)
		{
			SDL_WaitEvent(&event);
			if (event.type == SDL_EVENT_QUIT)
			{
				NES::isRunning = false;
				isPaused = false;
			}
			else if (event.type == SDL_EVENT_KEY_DOWN)
			{
				switch (event.key.key)
				{
				case SDLK_P:
					isPaused = false;
					break;

				case SDLK_ESCAPE:
					NES::isRunning = false;
					isPaused = false;
					break;
				}
			}
		}
	}

}