#pragma once
#include <string>

const int VIDEO_HEIGHT = 240;
const int VIDEO_WIDTH = 256;

const int PALETTES_ADDRESS = 0x3F00;

const int NAMETABLE1_ADDRESS = 0x2000;
const int NAMETABLE2_ADDRESS = 0x2400;
const int NAMETABLE3_ADDRESS = 0x2800;
const int NAMETABLE4_ADDRESS = 0x2C00;
const int NAMETABLE_SIZE = 0x400;

class CPU;

class PPU {
public:
	// Address Space
	uint8_t memory[0x3FFF + 1]{};
	// OAM Address Space
	uint8_t oam[256];
	// Display Screen
	uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT]{};

	int prgSize{}; // PRG ROM size in 16 KiB units
	int chrSize{}; // CHR ROM size in 8 KiB units(0 = board uses CHR RAM)

	// Address set by PPUADDR register
	uint16_t vramAddress{};

	int cycles{};
	int scanlines{};

	void loadROM(std::string filePath);

	// Load pattern table into video
	void loadPatternTable();

	PPU();
	~PPU();
	
	uint8_t writeMemoryPpu(uint16_t address, uint8_t data, CPU* cpu);
	uint8_t readMemoryPpu(uint16_t address, CPU* cpu);

	void renderFrame(CPU* cpu);
	void renderScanline();
private:
	bool isHighByte{};
	void vramIncrease(CPU* cpu);

	void renderSprite(int spriteIndex, int* videoX, int* videoY);
	void renderSprite(int spriteIndex, int videoX, int videoY);
};