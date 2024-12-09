#include "Cartridge.h"

Cartridge::Cartridge(int _prgSize, int _chrSize)
	:
	prgSize(_prgSize),
	chrSize(_chrSize),
	prg(_prgSize * 0x4000),
	chr(_chrSize * 0x2000)
{}

Cartridge::~Cartridge() {}

void Cartridge::loadPrg(char* buffer) {
	for (int i = 0; i < prg.size(); i++)
	{
		prg.at(i) = buffer[i + 16];
	}
}

void Cartridge::loadChr(char* buffer) {
	for (int i = 0; i < chr.size(); i++)
	{
		chr.at(i) = buffer[(prgSize * 0x4000) + i + 16];
	}
}