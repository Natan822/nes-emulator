#include "../Cartrige/Cartridge.h"

#include "Mapper.h"
#include "Mapper002.h"

Mapper002::Mapper002(CPU* cpu, PPU* ppu) : Mapper(cpu, ppu) {}
Mapper002::~Mapper002() {}

uint8_t Mapper002::prgRead(uint16_t address) const {
	// 0x8000-0xBFFF: 16 KB switchable PRG ROM bank
	if (address < 0xC000)
	{
		return this->cartridge->prg.at((address - 0x8000) | (bankSelect * 0x4000));
	}
	// 0xC000-0xFFFF: 16 KB PRG ROM bank, fixed to the last bank
	else 
	{
		return this->cartridge->prg.at((address - 0x8000) | ((cartridge->prgSize - 1) * 0x4000));
	}
}

void Mapper002::prgWrite(uint16_t address, uint8_t data) {
	this->bankSelect = data;
}

uint8_t Mapper002::chrRead(uint16_t address) const {
	return Mapper::chrRead(address);
}

void Mapper002::chrWrite(uint16_t address, uint8_t data) const {
	Mapper::chrWrite(address, data);
}