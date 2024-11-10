#include "../CPU/CPU.h"
#include "../PPU/PPU.h"
#include "../Graphics/Graphics.h"
#include "../Input/Input.h"
#include "../Controller/Controller.h"
#include "NES.h"

#include <chrono>
#include <iostream>

NES::NES() : NES(1, 1) {}
NES::NES(int _windowScale) : NES(_windowScale, 1) {}
NES::NES(int _windowScale, int speed) 
	:	ppu(new PPU()),
		controller(new Controller()),
		cpu(new CPU(*ppu, *controller)),
		windowScale(_windowScale)
{
	this->frameDelay = 16.667 / speed;
}

NES::~NES() {
	delete ppu;
	delete cpu;
	delete controller;
}

void NES::loadROM(std::string path) {
	try {
		ppu->loadROM(path);
		cpu->loadROM(path);
	}
	catch (std::runtime_error& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void NES::start() {
	Graphics::initialize((VIDEO_WIDTH * windowScale), (VIDEO_HEIGHT * windowScale), VIDEO_WIDTH, VIDEO_HEIGHT);

	int oldCycles = 0;
	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;
	while (!quit)
	{
		auto currentCycleTime = std::chrono::high_resolution_clock::now();
		float delta = std::chrono::duration<float, std::chrono::milliseconds::period>(currentCycleTime - lastCycleTime).count();

		cpu->cycle();

		if ((cpu->cycles - oldCycles) >= 113)
		{
			if (ppu->scanlines < 240)
			{
				ppu->renderScanline();
			}
			else
			{
				if (ppu->scanlines == 260)
				{
					ppu->scanlines = 0;
					cpu->writeMemory(PPUSTATUS, ppu->regPpuStatus & ~0x40);
				}
				ppu->scanlines++;
			}

			if (ppu->scanlines == 240)
			{
				if (delta >= this->frameDelay)
				{
					quit = Input::inputProcessing(this->controller);
					ppu->renderFrame(cpu);
					lastCycleTime = currentCycleTime;
				}
			}
			oldCycles = cpu->cycles;
		}
	}
	Graphics::shutdown();
}