#pragma once

#include <string>

class CPU;
class PPU;

class NES {
public:
	PPU *ppu;
	CPU *cpu;

	NES();
	NES(int _windowScale);
	NES(int _windowScale, int speed);
	~NES();

	void loadROM(std::string path);
	void start();

private:
	int windowScale;
	float frameDelay;
};