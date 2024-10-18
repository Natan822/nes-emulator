#pragma once
#include <string>
#include "../CPU/CPU.h"

const int VIDEO_HEIGHT = 240;
const int VIDEO_WIDTH = 256;

class PPU {
public:
	// Registers
	uint8_t* ppuCtrl;
	uint8_t* ppuMask;
	uint8_t* ppuStatus;
	uint8_t* oamAddr;
	uint8_t* oamData;
	uint8_t* ppuScroll;
	uint8_t* ppuAddr;
	uint8_t* ppuData;
	uint8_t* oamDma;

	// Address Space
	uint8_t memory[0x3FFF + 1]{};
	// Display Screen
	uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT]{};

	int prgSize{}; // PRG ROM size in 16 KiB units
	int chrSize{}; // CHR ROM size in 8 KiB units(0 = board uses CHR RAM)

	void loadROM(std::string filePath);

	// Load pattern table into video
	void loadPatternTable();

	PPU(CPU& cpu);
	~PPU();
private:
};