#include "APU.h"

void APU::Pulse::update(uint16_t address, uint8_t data) {
	this->sequencerPosition = 0x80;
	switch (address & 0x3)
	{
	case 0:
		duty = data >> 6;
		envelope.loopFlag = data & 0x20;
		envelope.constantVolume = data & 0x10;
		envelope.volume = data & 0xF;
		envelope.startFlag = 1;
		envelope.divider.period = envelope.volume;

		sequencerOutput = dutyTable[duty] & sequencerPosition;
		break;
	case 1:
		sweep.enabledFlag = data & 0x80;
		sweep.divider.period = ((data & 0x70) >> 4) + 1;
		sweep.negateFlag = data & 0x8;
		sweep.shiftCount = data & 0x7;

		sweep.reloadFlag = true;
		updateTargetPeriod(address < 0x4004);
		break;
	case 2:
		initialTimer &= 0x700;
		initialTimer |= data;
	
		break;
	case 3:
		if (isEnabled)
		{
			lengthCounter = lengthTable[data >> 3];
		}
		initialTimer &= 0xFF;
		initialTimer |= (data & 0x7) << 8;

		envelope.startFlag = true;
		sequencerPosition = 0x80;
		sequencerOutput = dutyTable[duty] & sequencerPosition;
		break;
	}
}

void APU::Pulse::clock(bool isPulse1) {
	if (currentTimer > 0)
	{
		currentTimer--;
	}
	else
	{
		currentTimer = initialTimer;
		sequencerPosition >>= 1;
		if (sequencerPosition == 0)
		{
			sequencerPosition = 0x80;
		}
		sequencerOutput = dutyTable[duty] & sequencerPosition;
	}

	updateTargetPeriod(isPulse1);
}

void APU::Pulse::sweepClock() {
	sweep.divider.counter--;
	if (sweep.divider.counter == 0)
	{
		if (sweep.shiftCount > 0 && sweep.enabledFlag && initialTimer >= 8 && sweep.targetPeriod <= 0x7FF)
		{
			initialTimer = sweep.targetPeriod;
		}
		sweep.divider.counter = sweep.divider.period;
	}
	if (sweep.reloadFlag)
	{
		sweep.divider.counter = sweep.divider.period;
		sweep.reloadFlag = false;
	}
}

void APU::Pulse::lengthCounterClock() {
	if (lengthCounter > 0 && !envelope.loopFlag)
	{
		lengthCounter--;
	}
}

void APU::Pulse::updateTargetPeriod(bool isPulse1) {
	uint16_t changeAmount = initialTimer >> sweep.shiftCount;
	if (sweep.negateFlag)
	{
		sweep.targetPeriod = initialTimer - changeAmount;
		if (isPulse1)
		{
			sweep.targetPeriod++;
		}
	}
	else
	{
		sweep.targetPeriod = initialTimer + changeAmount;
	}
}

// Ref.: https://forums.nesdev.org/viewtopic.php?f=3&t=13767
bool APU::Pulse::isSweepMuting() {
	if (initialTimer < 8 || (!sweep.negateFlag && sweep.targetPeriod > 0x7FF))
	{
		return true;
	}
	return false;
}