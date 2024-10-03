#include <stdint.h>
#include "CPU.h"

CPU::CPU() {
	sp = STACK_START_ADDRESS;
}

void CPU::push(uint8_t value) {
	memory[sp] = value;
	sp--;
}

uint8_t CPU::pop() {
	sp++;
	return memory[sp];
}

void CPU::setFlag(char flag, uint8_t value) {
	switch (flag)
	{
	case 'N':
		status &= ~0x80;
		if (value)
		{
			status |= 0x80;
		}
		break;

	case 'V':
		status &= ~0x40;
		if (value)
		{
			status |= 0x40;
		}
		break;

	case 'B':
		status &= ~0x10;
		if (value)
		{
			status |= 0x10;
		}
		break;

	case 'D':
		status &= ~0x8;
		if (value)
		{
			status |= 0x8;
		}
		break;

	case 'I':
		status &= ~0x4;
		if (value)
		{
			status |= 0x4;
		}
		break;

	case 'Z':
		status &= ~0x2;
		if (value)
		{
			status |= 0x2;
		}
		break;

	case 'C':
		status &= ~0x1;
		if (value)
		{
			status |= 0x1;
		}
		break;

	default:
		break;
	}
}

bool CPU::getFlag(char flag) {
	switch (flag)
	{
	case 'N':
		return status & 0x80;

	case 'V':
		return status & 0x40;

	case 'B':
		return status & 0x10;

	case 'D':
		return status & 0x8;

	case 'I':
		return status & 0x4;

	case 'Z':
		return status & 0x2;

	case 'C':
		return status & 0x1;

	default:
		break;
	}
	return NULL;
}

