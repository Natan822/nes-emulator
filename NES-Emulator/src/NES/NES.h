#pragma once

#include <string>

class APU;
class CPU;
class PPU;
class Controller;

class NES {
public:
	static bool isRunning;

	APU* apu;
	PPU* ppu;
	Controller* controller;
	CPU* cpu;

	NES();
	NES(int _windowScale);
	NES(int _windowScale, double speed);
	~NES();

	void loadROM(std::string path);
	void start();

	void clock(bool updateFrame = true);
private:
	int windowScale;
	double frameDelay;
	bool isRomLoaded;
};