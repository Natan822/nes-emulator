#include "../PPU/PPU.h"
#include "../Controller/Controller.h"
#include "../Mappers/Mapper.h"
#include "CPU.h"

uint8_t CPU::writeMemory(uint16_t address, uint8_t data) {
	// PPU registers and mirrors
	if (address >= 0x2000 && address < 0x4000)
	{
		stepPpu();
		return ppu.writeMemoryPpu(0x2000 | (address & 7), data, this);
	}
	// OAMDMA PPU register
	else if (address == 0x4014)
	{
		stepPpu();
		return ppu.writeMemoryPpu(address, data, this);
	}
	// Controller input
	else if (address == 0x4016)
	{
		controllerInput = this->controller.getInput();
	}
	else if (address >= 0x8000)
	{
		this->mapper->cpuWrite(address, data);
		return data;
	}
	memory.at(address) = data;
	return data;
}

uint8_t CPU::readMemory(uint16_t address) {
	// PPU registers and mirrors
	if (address >= 0x2000 && address < 0x4000)
	{
		stepPpu();
		return ppu.readMemoryPpu(0x2000 | (address & 7), this);
	}
	// OAMDMA PPU register
	else if (address == 0x4014)
	{
		stepPpu();
		return ppu.readMemoryPpu(address, this);
	}
	// Input
	else if (address == 0x4016)
	{
		uint8_t data = controllerInput & 0x1;
		controllerInput >>= 1;
		return data;
	}

	else if (address >= 0x8000)
	{
		return this->mapper->cpuRead(address);
	}

	return memory.at(address);
}

void CPU::push(uint8_t value) {
	memory.at(sp + STACK_BOTTOM_ADDRESS) = value;
	sp--;
}

uint8_t CPU::pop() {
	sp++;
	return memory.at(sp + STACK_BOTTOM_ADDRESS);
}