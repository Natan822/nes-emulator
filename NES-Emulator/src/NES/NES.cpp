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
		windowScale(_windowScale)
{
	this->frameDelay = 16666.667 / speed;
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
	bool quit = false;

	auto lastFrameTime = std::chrono::high_resolution_clock::now();
	double delta;
	while (!quit)
	{
		cpu->cycle();
		if ((ppu->cycles - oldCycles) >= 341)
		{
			if (ppu->scanlines < 240)
			{
				if (ppu->enableBackground)
				{
					ppu->renderScanline();
				}
				else
				{
					ppu->scanlines++;
				}
			}
			else
			{
				if (ppu->scanlines == 240)
				{
					delta = std::chrono::duration<float, std::chrono::microseconds::period>
						(std::chrono::high_resolution_clock::now() - lastFrameTime).count();
					if (delta < this->frameDelay)
					{
						std::this_thread::sleep_for(std::chrono::duration<double, std::micro>
							(this->frameDelay - delta));
					}
					ppu->renderFrame(cpu);
					lastFrameTime = std::chrono::high_resolution_clock::now();

					quit = Input::inputProcessing(this->controller);
					ppu->scanlines++;
				}

				else if (ppu->scanlines == 261)
				{
					ppu->scanlines = 0;
					cpu->writeMemory(PPUSTATUS, ppu->regPpuStatus & ~0x40);
				}
				else
				{
					ppu->scanlines++;
				}
			}
			oldCycles = ppu->cycles;
		}
	}
	Graphics::shutdown();
}