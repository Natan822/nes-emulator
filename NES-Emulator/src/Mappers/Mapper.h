#pragma once
#include <memory>

class CPU;
class PPU;

class Mapper {
public:

	CPU* cpu;
	PPU* ppu;
	std::shared_ptr<Cartridge> cartridge;

	Mapper(CPU* cpu, PPU* ppu);
	~Mapper();

	virtual uint8_t prgRead(uint16_t address) const;
	virtual void prgWrite(uint16_t address, uint8_t data);

	virtual uint8_t chrRead(uint16_t address) const;
	virtual void chrWrite(uint16_t address, uint8_t data) const;
};