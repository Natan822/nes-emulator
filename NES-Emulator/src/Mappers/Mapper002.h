#pragma once

class Mapper002 : public Mapper {
public:
	uint8_t bankSelect{};

	Mapper002(CPU* cpu, PPU* ppu);
	~Mapper002();

	uint8_t prgRead(uint16_t address) const override;
	void prgWrite(uint16_t address, uint8_t data) override;

	uint8_t chrRead(uint16_t address) const override;
	void chrWrite(uint16_t address, uint8_t data) const override;
};