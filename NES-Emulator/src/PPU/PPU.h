#pragma once

#include <memory>
#include <string>
#include <vector>
#include <chrono>

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
class Mapper;
class Cartridge;

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

	enum RenderState {
		PRE_RENDER,
		RENDER,
		POST_RENDER
	};
	RenderState renderState;

	// Variables controlled by PPUCTRL
	int baseNametableAddress{};
	int spritePatternTableAddress{};
	int backgroundPatternTableAddress{};

	// Variables controlled by PPUMASK
	bool isGrayscale{};
	bool showLeftmostBackground{};
	bool showLeftmostSprites{};
	bool enableBackground{};
	bool enableSprites{};
	bool isRedEmphasized{};
	bool isGreenEmphasized{};
	bool isBlueEmphasized{};

	// Address Space
	std::vector<uint8_t> memory;
	// OAM Address Space
	std::vector<uint8_t> oam;
	// CHR memory
	std::vector<uint8_t> chr;
	// Display Screen
	std::vector<uint32_t> video;
	std::vector<uint8_t> backgroundPixelBits;
	unsigned int backgroundIndex{};

	int prgSize{}; // PRG ROM size in 16 KiB units
	int chrSize{}; // CHR ROM size in 8 KiB units(0 = board uses CHR RAM)

	// Address set by PPUADDR register
	uint16_t vramAddress{};

	uint16_t vRegister{};
	uint16_t tRegister{};
	uint8_t xRegister{};

	unsigned int cycles{};
	unsigned int scanlines{};
	unsigned int dot{};

	// Load pattern table into video
	void loadPatternTable();

	PPU();
	~PPU();
	
	uint8_t writeMemoryPpu(uint16_t address, uint8_t data, CPU* cpu);
	uint8_t readMemoryPpu(uint16_t address, CPU* cpu);
	uint8_t readBuffer{};

	void renderFrame();
	void renderScanline();
	void renderOAM();

	void step(CPU* cpu, bool updateFrame = true);

	std::shared_ptr<Cartridge> cartridge;
	std::shared_ptr<Mapper> mapper;

	uint8_t memoryRead(uint16_t address);
	void memoryWrite(uint16_t address, uint8_t data);

	uint8_t getPaletteIndex(int xQuadrant, int yQuadrant, uint8_t attributeByte);
	int getPixelColor(int pixelValue);

private:

	int spriteHeight{};

	bool isOddFrame{};

	// Load tiles data into shift registers
	void loadShiftRegisters(); 
	// Shift registers that hold the current and next tiles data
	uint16_t firstPlaneByteData{};
	uint16_t secondPlaneByteData{};

	uint8_t currentAttributeByte{};

	int coarseX{};
	int coarseY{};

	void preRender();
	void render();
	void postRender(CPU* cpu, bool updateFrame);

	void defineRenderState();

	void incrementCoarseX();
	void incrementY();

	bool isHighByte{};
	void vramIncrease(CPU* cpu);

	void renderSprite(int spriteIndex, int* videoX, int* videoY);
	// Render sprite at given X, Y location
	void renderSprite(int spriteIndex, int x, int y, int paletteIndex, bool vFlip, bool hFlip, bool isBehindBackground);
	// Render one pixel of a sprite at the last scanline rendered if that sprite has any pixels there. If a nontransparent pixel is drawn over a background's nontransparent pixel, returns true(made specifically for sprite 0 hit checking)
	bool renderSpritePixel(int spriteIndex, int x, int y, int paletteIndex, bool vFlip, bool hFlip, bool isBehindBackground);
	void handleSpriteZero();

	void updatePPUCTRL();
	void updatePPUMASK();
	void updatePPUSCROLL();

	int emphasizeRed(int color);
	int emphasizeGreen(int color);
	int emphasizeBlue(int color);

	void setPixel(int x, int y, int colorPixel);
	int getPixel(int x, int y);

	void mirrorPalettes();
	void writePalettes(uint16_t address, uint8_t data);
	uint8_t readPalettes(uint16_t address);
};