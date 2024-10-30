#include <fstream>
#include <stdint.h>
#include <string>
#include "../CPU/CPU.h"
#include "PPU.h"
#include "../Graphics/Graphics.h"

#include <iostream>

PPU::PPU() {
	cycles = 21;
	isHighByte = true;
}

PPU::~PPU() {}

void PPU::loadROM(std::string filePath) {
	std::ifstream file(filePath, std::ios::binary | std::ios::ate);

	if (!file.is_open())
	{
		throw std::runtime_error("Could not open file: " + filePath);
	}

	std::streampos size = file.tellg();
	char* buffer = new char[size];

	file.seekg(0, std::ios::beg);
	file.read(buffer, size);
	file.close();

	prgSize = static_cast<int>(static_cast<unsigned char>(buffer[4]));

	// Starts at 16 because the header bytes occupy the first 16 bytes
	for (int i = 16; i < 8192 + 16; i++)
	{	
		memory[i - 16] = buffer[(prgSize * 16384) + i];
	}

	delete[] buffer;

	// Temporary default palette table
	this->memory[PALETTES_ADDRESS] = 0x0E;
	for (int i = 1; i < 4; i++)
	{
		this->memory[PALETTES_ADDRESS + i] = 0x15 + i;
	}
}

void PPU::loadPatternTable() {
	int videoX = 0;
	int videoY = 0;

	for (int spriteIndex = 0; spriteIndex < 0x1FF; spriteIndex++)
	{
		renderSprite(spriteIndex, &videoX, &videoY);
	}
}

uint8_t PPU::writeMemoryPpu(uint16_t address, uint8_t data, CPU* cpu) {
	switch (address)
	{
	case PPUCTRL:
		// Vblank NMI enable
		cpu->nmiInterrupt = data & 0x80;
		break;

	case PPUMASK:
		break;

	case PPUSTATUS:
		break;

	case OAMADDR:
		break;

	case OAMDATA:
		// Write to OAM
		this->oam[cpu->memory[OAMADDR]] = data;
		// Increment OAMADDR
		cpu->memory[OAMADDR]++;
		break;

	case PPUSCROLL:
		break;

	case PPUADDR:
		if (isHighByte)
		{
			vramAddress = (vramAddress & 0xFF) | (data << 8);
		}
		else
		{
			vramAddress = (vramAddress & 0xFF00) | data;
		}
		isHighByte = !isHighByte;
		break;

	case PPUDATA:
		this->memory[vramAddress] = data;
		vramIncrease(cpu);
		break;

	case OAMDMA:
	{
		uint16_t sourceAddress = data << 8;
		// Copies from source address to OAM
		for (int byteIndex = 0; byteIndex < 256; byteIndex++)
		{
			this->oam[byteIndex] = cpu->memory[sourceAddress + byteIndex];
		}
		cpu->cycles += 513;
		break;
	}
	default:
		break;
	}

	cpu->memory[address] = data;
	return data;
}

uint8_t PPU::readMemoryPpu(uint16_t address, CPU* cpu) {
	uint8_t data = cpu->memory[address];
	switch (address)
	{
	case PPUCTRL:
		break;

	case PPUMASK:
		break;

	case PPUSTATUS:
		// Clear vblank_flag on read
		cpu->memory[address] &= ~0x80;

		isHighByte = false;
		break;

	case OAMADDR:
		break;

	case OAMDATA:
		break;

	case PPUSCROLL:
		break;

	case PPUADDR:
		break;

	case PPUDATA:
		data = this->memory[vramAddress];
		vramIncrease(cpu);
		break;

	case OAMDMA:
		break;

	default:
		break;
	}

	return data;
}

void PPU::vramIncrease(CPU* cpu) {
	// Check VRAM address increment mode
	if (cpu->memory[PPUCTRL] & 0x4)
	{
		vramAddress += 32;
	}
	else
	{
		vramAddress++;
	}
	vramAddress &= 0x3FFF;
}

void PPU::renderFrame(CPU* cpu) {
	/*int videoX = 0;
	int videoY = 0;

	for (int address = 0; address < NAMETABLE_SIZE - 64; address++)
	{
		uint8_t spriteIndex = this->memory[address + NAMETABLE1_ADDRESS];
		renderSprite(spriteIndex, &videoX, &videoY);
	}*/
	update(this->video, (sizeof(this->video[0]) * VIDEO_WIDTH));
	cpu->memory[PPUSTATUS] |= 0x80;
	cpu->nmiInterrupt = true;
}

void PPU::renderScanline() {
	for (int videoX = 0; videoX < VIDEO_WIDTH; videoX += 8)
	{
		uint8_t spriteIndex = this->memory[NAMETABLE1_ADDRESS + videoX + ((scanlines / 8) * VIDEO_WIDTH)];
		uint16_t addressSprite = spriteIndex * 16;

		int videoY = scanlines % 8;

		uint8_t firstPlaneByte = this->memory[addressSprite + videoY];
		uint8_t secondPlaneByte = this->memory[addressSprite + videoY + 8];

		uint8_t byteMask = 0x80;
		for (int bit = 0; bit < 8; bit++)
		{
			uint8_t firstPlaneBit = (firstPlaneByte & byteMask) ? 1 : 0;
			uint8_t secondPlaneBit = (secondPlaneByte & byteMask) ? 1 : 0;

			uint8_t pixelBits = (secondPlaneBit << 1) | firstPlaneBit;
			uint8_t pixelValue = this->memory[PALETTES_ADDRESS + pixelBits];

			// Here "bit" works as an X offset and "byte" works as a Y offset for the sprite coordinates
			video[videoX + bit + (scanlines * VIDEO_WIDTH)] = colorsMapTable[pixelValue];

			byteMask >>= 1;
		}

	}
	scanlines++;
}

void PPU::renderSprite(int spriteIndex, int* videoX, int* videoY) {
	uint16_t addressSprite = spriteIndex * 16;
	for (int byte = 0; byte < 8; byte++)
	{
		uint8_t firstPlaneByte = this->memory[addressSprite + byte];
		uint8_t secondPlaneByte = this->memory[addressSprite + byte + 8];

		uint8_t byteMask = 0x80;
		for (int bit = 0; bit < 8; bit++)
		{
			uint8_t firstPlaneBit = (firstPlaneByte & byteMask) ? 1 : 0;
			uint8_t secondPlaneBit = (secondPlaneByte & byteMask) ? 1 : 0;

			uint8_t pixelBits = (secondPlaneBit << 1) | firstPlaneBit;
			uint8_t pixelValue = this->memory[PALETTES_ADDRESS + pixelBits];

			// Here "bit" works as an X offset and "byte" works as a Y offset for the sprite coordinates
			video[*videoX + bit + ((*videoY + byte) * VIDEO_WIDTH)] = colorsMapTable[pixelValue];

			byteMask >>= 1;
		}
	}
	*videoX += 8;
	if (*videoX >= VIDEO_WIDTH)
	{
		*videoX = 0;
		*videoY += 8;
	}
}

void PPU::renderSprite(int spriteIndex, int videoX, int videoY) {
	uint16_t addressSprite = spriteIndex * 16;
	for (int byte = 0; byte < 8; byte++)
	{
		uint8_t firstPlaneByte = this->memory[addressSprite + byte];
		uint8_t secondPlaneByte = this->memory[addressSprite + byte + 8];

		uint8_t byteMask = 0x80;
		for (int bit = 0; bit < 8; bit++)
		{
			uint8_t firstPlaneBit = (firstPlaneByte & byteMask) ? 1 : 0;
			uint8_t secondPlaneBit = (secondPlaneByte & byteMask) ? 1 : 0;

			uint8_t pixelBits = (secondPlaneBit << 1) | firstPlaneBit;
			uint8_t pixelValue = this->memory[PALETTES_ADDRESS + pixelBits];

			// Here "bit" works as an X offset and "byte" works as a Y offset for the sprite coordinates
			video[videoX + bit + ((videoY + byte) * VIDEO_WIDTH)] = colorsMapTable[pixelValue];

			byteMask >>= 1;
		}
	}
}