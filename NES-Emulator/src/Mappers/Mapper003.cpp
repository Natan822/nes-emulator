#include "../Cartrige/Cartridge.h"
#include "../CPU/CPU.h"
#include "../PPU/PPU.h"
#include "Mapper.h"
#include "Mapper003.h"
#include <stdint.h>
#include <iostream>

Mapper003::Mapper003(CPU* cpu, PPU* ppu) : Mapper(cpu, ppu) {}
Mapper003::~Mapper003() {}

uint8_t Mapper003::prgRead(uint16_t address) const {
	return Mapper::prgRead(address);
}
void Mapper003::prgWrite(uint16_t address, uint8_t data) {
	this->bankSelect = data & 0x3;
}

uint8_t Mapper003::chrRead(uint16_t address) const {
	return cartridge->chr.at((address & 0x1FFF) | (bankSelect * 0x2000));
}
void Mapper003::chrWrite(uint16_t address, uint8_t data) const {
	cartridge->chr.at((address & 0x1FFF) | (bankSelect * 0x2000)) = data;
}