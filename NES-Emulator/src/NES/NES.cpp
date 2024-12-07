#include "../CPU/CPU.h"
#include "../PPU/PPU.h"
#include "../Graphics/Graphics.h"
#include "../Input/Input.h"
#include "../Controller/Controller.h"
#include "NES.h"

#include <chrono>
#include <iostream>
#include <thread>

NES::NES() : NES(1, 1) {}
NES::NES(int _windowScale) : NES(_windowScale, 1) {}
NES::NES(int _windowScale, double speed) 
	:	ppu(new PPU()),
		controller(new Controller()),
		cpu(new CPU(*ppu, *controller)),
		windowScale(_windowScale),
		isRomLoaded(false),
		frameDelay(16000.0 / speed)
{}

NES::~NES() {
	delete ppu;
	delete cpu;
	delete controller;
}

void NES::loadROM(std::string path) {
	try {
		ppu->loadROM(path);
		cpu->loadROM(path);
		isRomLoaded = true;
	}
	catch (std::runtime_error& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void NES::start() {
	if (!isRomLoaded)
	{
		std::cout << "Error: Unable to start NES, no ROM's have been loaded." << std::endl; 
		return;
	}
	Graphics::initialize((VIDEO_WIDTH * windowScale), (VIDEO_HEIGHT * windowScale), VIDEO_WIDTH, VIDEO_HEIGHT);

	int oldCycles = 0;
	bool quit = false;

	auto lastFrameTime = std::chrono::high_resolution_clock::now();
	double delta;

	int masterClock = 0;
	while (!quit)
	{
		if (masterClock % 12 == 0)
		{
			cpu->step();
		}
		if (masterClock % 4 == 0)
		{
			ppu->step(cpu);
		}
		masterClock++;
		if (masterClock > 12)
		{
			masterClock = 0;
		}
	}
	Graphics::shutdown();
}