#include "../Controller/Controller.h"
#include "../NES/NES.h"
#include "SDL.h"
#include "Input.h"
#include "../Graphics/Graphics.h"

namespace Input {
	bool inputProcessing(Controller* controller) {
		bool quit = false;

		SDL_Event event;

		while (SDL_PollEvent(&event)) 
		{
			switch (event.type)
			{
			case SDL_QUIT:
				NES::isRunning = false;
				break;
			case SDL_KEYDOWN:
				handleKeyDown(controller, event, &quit);
				break;
			case SDL_KEYUP:
				handleKeyUp(controller, event);
				break;
			}
		}
		return quit;
	}

	void handleKeyDown(Controller* controller, SDL_Event event, bool* quit) {
		switch (event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			NES::isRunning = false;
			break;
		case SDLK_p:
			pause();
			break;
		case SDLK_w:
			controller->buttonPress('U');
			break;
		case SDLK_s:
			controller->buttonPress('D');
			break;
		case SDLK_a:
			controller->buttonPress('L');
			break;
		case SDLK_d:
			controller->buttonPress('R');
			break;
		case SDLK_o:
			controller->buttonPress('S');
			break;
		case SDLK_i:
			controller->buttonPress('E');
			break;
		case SDLK_k:
			controller->buttonPress('B');
			break;
		case SDLK_l:
			controller->buttonPress('A');
			break;
		}
	}

	void handleKeyUp(Controller* controller, SDL_Event event) {
		switch (event.key.keysym.sym)
		{
		case SDLK_w:
			controller->buttonRelease('U');
			break;
		case SDLK_s:
			controller->buttonRelease('D');
			break;
		case SDLK_a:
			controller->buttonRelease('L');
			break;
		case SDLK_d:
			controller->buttonRelease('R');
			break;
		case SDLK_o:
			controller->buttonRelease('S');
			break;
		case SDLK_i:
			controller->buttonRelease('E');
			break;
		case SDLK_k:
			controller->buttonRelease('B');
			break;
		case SDLK_l:
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
			if (event.type == SDL_QUIT)
			{
				NES::isRunning = false;
				isPaused = false;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_p:
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