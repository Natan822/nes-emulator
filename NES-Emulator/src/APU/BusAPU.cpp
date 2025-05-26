#include <stdint.h>
#include "APU.h"
#include "../Audio/Audio.h"

uint8_t APU::readMemoryApu(uint16_t address) {
	if (address == 0x4015)
	{
		/*
			TODO 
			{
				- DMC interrupt (I)
				- DMC active (D)
			}
		*/
		bool oldFrameInterrupt = m_frameInterrupt;
		m_frameInterrupt = false;
		return 
			(oldFrameInterrupt << 7) | 
			((pulse2.lengthCounter > 0) << 1) |
			(pulse1.lengthCounter > 0);
	}
	return 0;
}

uint8_t APU::writeMemoryApu(uint16_t address, uint8_t data) {
	// Pulse 1
	if (address <= 0x4003)
	{
		pulse1.update(address, data);
	}
	// Pulse 2
	else if (address <= 0x4007) 
	{
		pulse2.update(address, data);
	}
	// Triangle
	else if (address == 0x4008 || address == 0x400A || address == 0x400B)
	{
	}
	// Status
	else if (address == 0x4015)
	{
		updateStatus(data);
	}
	// Frame Counter
	else if (address == 0x4017)
	{
		m_sequencerMode = data & 0x80;
		m_interruptInhibit = data & 0x40;
		if (m_interruptInhibit)
		{
			m_frameInterrupt = false;
		}
		m_shouldResetFrameCounter = true;
		m_cyclesUntilReset = m_currentCycleState == ACTIVE ? 3 : 4;

		if (m_sequencerMode)
		{
			quarterFrame();
			halfFrame();
		}
	}

	return data;
}

void APU::updateStatus(uint8_t data) {
	pulse1.isEnabled = data & 0x1;
	pulse2.isEnabled = data & 0x2;
	m_enableTriangle = data & 0x4;
	m_enableNoise = data & 0x8;
	m_enableDMC = data & 0x10;

	// Halt Length Counters
	if (!pulse1.isEnabled)
	{
		pulse1.lengthCounter = 0;
	}
	if (!pulse2.isEnabled)
	{
		pulse2.lengthCounter = 0;
	}
	/*
		TODO
		{
			- Halt Triangle
			- Halt Noise
			- Halt DMC
		}
	*/
}