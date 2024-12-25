#include "../Audio/Audio.h"
#include "SDL.h"
#include "../CPU/CPU.h"
#include "APU.h"

APU::APU() :
	soundBuffer(735)
{}

APU::~APU() {}

void APU::step(CPU* cpu) {
	//SDL_QueueAudio(Audio::device, soundBuffer, Audio::audioBufferSize);
	uint8_t mask = 0x80;
	for (int i = 0; i < 8; i++)
	{
		int sample = (dutyTable[pulse1.duty] & mask) ? 1 : 0;
		soundBuffer.at(index) = Audio::AMPLITUDE * sample;
		if (sample != 0)
		{
			int a = 2;
		}
		index++;
		if (index > 735)
		{
			index = 0;
		}
		mask >>= 1;
	}

	this->cycles++;
	if (this->cycles >= 14914 && !interruptInhibit)
	{
		cpu->irqInterrupt = true;
		if (this->cycles == 19415)
		{
			this->cycles = 1;
		}
	}
}