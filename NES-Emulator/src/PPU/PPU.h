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
const int ATTRIBUTE_TABLE_OFFSET = 960;

class CPU;

class PPU {
public:

	// Registers
	uint8_t regPpuCtrl{};
	uint8_t regPpuMask{};
	uint8_t regPpuStatus{};
	uint8_t regOamAddr{};
	uint8_t regOamData{};
	uint8_t regPpuScroll{};
	uint8_t regPpuAddr{};
	uint8_t regPpuData{};
	uint8_t regOamDma{};

	// Mirroring type
	enum Mirroring {
		HORIZONTAL,
		VERTICAL
	};
	Mirroring mirrorType{};
	int mirrorNametableAddress{};

	int fineY{};
	int coarseY{};

	// Variables controlled by PPUCTRL
	int baseNametableAddress{};
	int spritePatternTableAddress{};
	int backgroundPatternTableAddress{};

	// Variables controlled by PPUMASK
	bool isGrayscale{};
	bool isRedEmphasized{};
	bool isGreenEmphasized{};
	bool isBlueEmphasized{};

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

	unsigned int cycles{};
	int scanlines{};

	void loadROM(std::string filePath);

	// Load pattern table into video
	void loadPatternTable();

	PPU();
	~PPU();
	
	uint8_t writeMemoryPpu(uint16_t address, uint8_t data, CPU* cpu);
	uint8_t readMemoryPpu(uint16_t address, CPU* cpu);
	uint8_t readBuffer{};

	void renderFrame(CPU* cpu);
	void renderScanline();
	void renderOAM();
private:
	bool isHighByte{};
	void vramIncrease(CPU* cpu);

	void renderSprite(int spriteIndex, int* videoX, int* videoY);
	void renderSprite(int spriteIndex, int videoX, int videoY, int paletteIndex, bool vFlip, bool hFlip);
	bool renderSpriteHitDetect(int spriteIndex, int videoX, int videoY, int paletteIndex, bool vFlip, bool hFlip);

	void updatePPUCTRL();
	void updatePPUMASK();
	void updatePPUSCROLL();

	uint8_t getPaletteIndex(int xQuadrant, int yQuadrant, uint8_t attributeByte);
	int getPixelColor(int pixelValue);

	int emphasizeRed(int color);
	int emphasizeGreen(int color);
	int emphasizeBlue(int color);

	int getNametableAddress();

	void setPixel(int x, int y, int colorPixel);
	int getPixel(int x, int y);

	void mirrorPalettes();
	void writePalettes(uint8_t data);
	uint8_t readPalettes();
};