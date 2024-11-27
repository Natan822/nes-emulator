#include "../CPU/CPU.h"
#include "../PPU/PPU.h"
#include "Mapper.h"
#include "Mapper003.h"
#include <stdint.h>
#include <iostream>

Mapper003::Mapper003(CPU* cpu, PPU* ppu) : Mapper(cpu, ppu) {}
Mapper003::~Mapper003() {}

uint8_t Mapper003::cpuRead(uint16_t address) const {
	return cpu->memory.at(address);
}
void Mapper003::cpuWrite(uint16_t address, uint8_t data) {
	this->bankSelect = data & 0x3;
	std::cout << std::hex << static_cast<int>(bankSelect) << std::endl;
}

uint8_t Mapper003::chrRead(uint16_t address) const {
	return ppu->chr.at((address & 0x1FFF) | (bankSelect * 0x2000));
}
void Mapper003::chrWrite(uint16_t address, uint8_t data) const {
	//ppu->chr.at((address & 0x1FFF) | (bankSelect * 0x2000)) = data;
}