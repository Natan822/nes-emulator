#include "APU.h"
#include <iostream>

uint8_t APU::Envelope::output() {
	if (constantVolume)
	{
		return volume;
	}
	return decayLevelCounter;
}

void APU::Envelope::clock() {
	if (startFlag)
	{
		startFlag = false;
		decayLevelCounter = 15;
		divider.counter = divider.period;
	}
	else
	{
		if (divider.clock() && loopFlag)
		{
			if (decayLevelCounter > 0)
			{
				decayLevelCounter--;
			}
			else
			{
				//loopFlag = true;
				decayLevelCounter = 15;
			}
		}
	}
}