#include "../APU/APU.h"
#include "../CPU/CPU.h"
#include "../PPU/PPU.h"
#include "../Audio/Audio.h"
#include "../Graphics/Graphics.h"
#include "../Input/Input.h"
#include "../Controller/Controller.h"
#include "NES.h"

#include <chrono>
#include <iostream>
#include <thread>

bool NES::isRunning = false;

NES::NES() : NES(1, 1) {}
NES::NES(int _windowScale) : NES(_windowScale, 1) {}
NES::NES(int _windowScale, double speed) :
	apu(new APU()),
	ppu(new PPU()),
	controller(new Controller()),
	cpu(new CPU(*ppu, *controller, *apu)),
	windowScale(_windowScale),
	isRomLoaded(false),
	frameDelay(16000.0 / speed)
{}

NES::~NES() {
	delete apu;
	delete ppu;
	delete cpu;
	delete controller;
}

void NES::loadROM(std::string path) {
	try {
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
	Audio::initialize();

	int oldCycles = 0;
	auto lastFrameTime = std::chrono::high_resolution_clock::now();
	double delta;

	isRunning = true;
	int masterClock = 0;
	while (isRunning)
	{
		apu->step(cpu);
		cpu->step();
		ppu->step(cpu);
		ppu->step(cpu);
		ppu->step(cpu);
		cpu->step();
		ppu->step(cpu);
		ppu->step(cpu);
		ppu->step(cpu);
		//if (masterClock % 12 == 0)
		//{
		//	cpu->step();
		//}
		//if (masterClock % 4 == 0)
		//{
		//	ppu->step(cpu);
		//}
		//masterClock++;
		//if (masterClock >= 12)
		//{
		//	masterClock = 0;
		//}
	}
	Graphics::shutdown();
}
