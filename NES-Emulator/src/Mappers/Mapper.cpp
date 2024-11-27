#include "../CPU/CPU.h"
#include "../PPU/PPU.h"
#include "Mapper.h"
#include <stdint.h>
#include <vector>

Mapper::Mapper(CPU* cpu, PPU* ppu)
	:	cpu(cpu),
		ppu(ppu)
{}
Mapper::~Mapper() {}

uint8_t Mapper::cpuRead(uint16_t address) const {
	return cpu->memory.at(address);
}
void Mapper::cpuWrite(uint16_t address, uint8_t data) {
	cpu->memory.at(address) = data;
}

uint8_t Mapper::chrRead(uint16_t address) const {
	return ppu->memory.at(address);
}
void Mapper::chrWrite(uint16_t address, uint8_t data) const {
	ppu->memory.at(address) = data;
}