#include "../PPU/PPU.h"
#include "../Controller/Controller.h"
#include "CPU.h"

uint8_t CPU::writeMemory(uint16_t address, uint8_t data) {
	if ((address >= 0x2000 && address <= 0x2007) || address == 0x4014)
	{
		return ppu.writeMemoryPpu(address, data, this);
	}
	else if (address == 0x4016)
	{
		controllerInput = this->controller.getInput();
	}
	memory.at(address) = data;
	return data;
}

uint8_t CPU::readMemory(uint16_t address) {
	if ((address >= 0x2000 && address <= 0x2007) || address == 0x4014)
	{
		return ppu.readMemoryPpu(address, this);
	}
	if (address == 0x4016)
	{
		uint8_t data = controllerInput & 0x1;
		controllerInput >>= 1;
		return data;
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