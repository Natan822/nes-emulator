#pragma once
#include <SDL3/SDL.h>

class Controller;

namespace Input {
	void inputProcessing(Controller* controller);
	void handleKeyDown(Controller* controller, SDL_Event event);
	void handleKeyUp(Controller* controller, SDL_Event event);
	void pause();
}