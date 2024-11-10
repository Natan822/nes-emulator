#pragma once

#include <string>

class CPU;
class PPU;
class Controller;

class NES {
public:
	PPU* ppu;
	Controller* controller;
	CPU* cpu;

	NES();
	NES(int _windowScale);
	NES(int _windowScale, float speed);
	~NES();

	void loadROM(std::string path);
	void start();

private:
	int windowScale;
	float frameDelay;
};