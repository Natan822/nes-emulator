#pragma once

#include <stdint.h>

class Controller {
public:
	Controller();
	~Controller();

	void buttonPress(char button);
	void buttonRelease(char button);
	uint8_t getInput();

private:
	bool up{};
	bool down{};
	bool left{};
	bool right{};
	bool start{};
	bool select{};
	bool b{};
	bool a{};
};