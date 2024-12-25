#pragma once
#include <vector>
#include <stdint.h>
#include "../CPU/CPU.h"

class APU {

	struct Pulse {
		uint8_t duty;
		bool envelopeLoop;
		bool constantVolume;
		uint8_t envelope;
		bool sweepUnitEnabled;
		uint8_t period;
		bool negate;
		uint8_t shift;
		uint8_t lengthCounterLoad;
		uint16_t timer;
	};

	uint8_t dutyTable[4] =
	{
		0b01000000,
		0b01100000,
		0b01111000,
		0b10011111
	};

public:
	
	APU();
	~APU();

	uint8_t readMemoryApu(uint16_t address);
	uint8_t writeMemoryApu(uint16_t address, uint8_t data);

	void step(CPU* cpu);
	std::vector<uint8_t> soundBuffer;

	unsigned int cycles{};
private:
	// Frame Counter flags
	bool sequencerMode{}; // 0 = 4-step sequence; 1 = 5-step sequence
	bool interruptInhibit{};
	bool frameInterrupt{};

	uint8_t index{};

	Pulse pulse1;
	Pulse pulse2;
	void updatePulse(Pulse* pulse, uint16_t address, uint8_t data);
};