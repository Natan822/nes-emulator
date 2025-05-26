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
		divider.counter--;
		if (divider.counter < 0)
		{
			divider.counter = divider.period;
			if (divider.counter > 0)
			{
				divider.counter--;
			}
			else if (loopFlag)
			{
				decayLevelCounter = 15;
			}
		}
	}
}