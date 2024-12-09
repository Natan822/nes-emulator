#include "../Cartrige/Cartridge.h"
#include "../CPU/CPU.h"
#include "../PPU/PPU.h"
#include "Mapper.h"
#include <stdint.h>
#include <vector>

Mapper::Mapper(CPU* cpu, PPU* ppu)
	:	cpu(cpu),
		ppu(ppu),
		cartridge(cpu->cartridge)
{}
Mapper::~Mapper() {}

uint8_t Mapper::prgRead(uint16_t address) const {
	return cartridge->prg.at((address - 0x8000) & ((cartridge->prgSize * 0x4000) - 1));
}
void Mapper::prgWrite(uint16_t address, uint8_t data) {
	cartridge->prg.at((address - 0x8000) & ((cartridge->prgSize * 0x4000) - 1)) = data;
}

uint8_t Mapper::chrRead(uint16_t address) const {
	return cartridge->chr.at(address);
}
void Mapper::chrWrite(uint16_t address, uint8_t data) const {
	cartridge->chr.at(address) = data;
}