#include <stdint.h>
#include "APU.h"
#include "../Audio/Audio.h"

uint8_t APU::readMemoryApu(uint16_t address) { return 0; }

uint8_t APU::writeMemoryApu(uint16_t address, uint8_t data) {
	// Pulse 1
	if (address <= 0x4003)
	{
		updatePulse(&pulse1, address, data);
	}
	// Pulse 2
	else if (address <= 0x4007) 
	{
		updatePulse(&pulse2, address, data);
	}
	// Frame Counter
	else if (address == 0x4017)
	{
		sequencerMode = data & 0x80;
		interruptInhibit = data & 0x40;
		if (interruptInhibit)
		{
			frameInterrupt = false;
		}
	}

	return data;
}

void APU::updatePulse(Pulse* pulse, uint16_t address, uint8_t data) {
	switch (address & 0x3)
	{
	case 0:
		pulse->duty = data >> 6;
		pulse->envelopeLoop = data & 0x20;
		pulse->constantVolume = data & 0x10;
		pulse->envelope = data & 0xF;
		break;
	case 1:
		pulse->sweepUnitEnabled = data & 0x80;
		pulse->period = (data & 0x70) >> 4;
		pulse->negate = data & 0x8;
		pulse->shift = data & 0x7;
		break;
	case 2:
		pulse->timer &= 0x700;
		pulse->timer |= data;
		break;
	case 3:
		pulse->lengthCounterLoad = (data & 0x1F) >> 3;
		pulse->timer &= 0xFF;
		pulse->timer |= (data & 0x7) << 8;
		break;
	}
}