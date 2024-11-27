#pragma once

class CPU;
class PPU;

class Mapper {
public:

	CPU* cpu;
	PPU* ppu;

	Mapper(CPU* cpu, PPU* ppu);
	~Mapper();

	virtual uint8_t cpuRead(uint16_t address) const;
	virtual void cpuWrite(uint16_t address, uint8_t data);

	virtual uint8_t chrRead(uint16_t address) const;
	virtual void chrWrite(uint16_t address, uint8_t data) const;
};