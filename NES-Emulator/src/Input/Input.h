#pragma once
#include <SDL.h>

class Controller;

namespace Input {
	bool inputProcessing(Controller* controller);
	void handleKeyDown(Controller* controller, SDL_Event event, bool* quit);
	void handleKeyUp(Controller* controller, SDL_Event event);
	void pause();
}