#pragma once

class Mapper003 : public Mapper {
public:

	uint8_t bankSelect{};

	Mapper003(CPU* cpu, PPU* ppu);
	~Mapper003();

	uint8_t cpuRead(uint16_t address) const override;
	void cpuWrite(uint16_t address, uint8_t data) override;

	uint8_t chrRead(uint16_t address) const override;
	void chrWrite(uint16_t address, uint8_t data) const override;

};