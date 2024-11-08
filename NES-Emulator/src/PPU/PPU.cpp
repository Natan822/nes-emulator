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

	mirrorType = buffer[6] & 0x1 ? HORIZONTAL : VERTICAL;

	prgSize = static_cast<int>(static_cast<unsigned char>(buffer[4]));

	// Starts at 16 because the header bytes occupy the first 16 bytes
	for (int i = 16; i < 8192 + 16; i++)
	{	
		memory[i - 16] = buffer[(prgSize * 16384) + i];
	}

	delete[] buffer;

	//// Temporary default palette table
	//this->memory[PALETTES_ADDRESS] = 0x0E;
	//for (int i = 1; i < 4; i++)
	//{
	//	this->memory[PALETTES_ADDRESS + i] = 0x15 + i;
	//}
}

void PPU::loadPatternTable() {
	int videoX = 0;
	int videoY = 0;

	for (int spriteIndex = 0; spriteIndex < 0x1FF; spriteIndex++)
	{
		renderSprite(spriteIndex, &videoX, &videoY);
	}
}

void PPU::renderFrame(CPU* cpu) {
	renderOAM();
	update(this->video, (sizeof(this->video[0]) * VIDEO_WIDTH));
	cpu->memory[PPUSTATUS] |= 0x80;
	regPpuStatus |= 0x80;

	if (regPpuCtrl & 0x80)
	{
		cpu->nmiInterrupt = true;
	}
}

void PPU::renderScanline() {
	int nametableIndex = 0;
	int nametableAddress = getNametableAddress();

	int scanlinesFineY = scanlines + fineY;
	for (int videoX = 0; videoX < VIDEO_WIDTH; videoX += 8)
	{
		uint8_t tileIndex;
		if (nametableAddress == mirrorNametableAddress)
		{
			tileIndex = 
				this->memory[nametableAddress + nametableIndex + (((scanlinesFineY - 240) / 8) * VIDEO_WIDTH / 8)];
		}
		else 
		{
			tileIndex = 
				this->memory[nametableAddress + nametableIndex + ((scanlinesFineY / 8) * VIDEO_WIDTH / 8)];
		}
		uint16_t addressSprite = (tileIndex * 16) + backgroundPatternTableAddress;

		int videoY = scanlines % 8;

		uint8_t firstPlaneByte = this->memory[addressSprite + videoY];
		uint8_t secondPlaneByte = this->memory[addressSprite + videoY + 8];

		uint8_t attributeByte;
		if (nametableAddress == mirrorNametableAddress)
		{
			attributeByte =
				this->memory[(nametableAddress + ATTRIBUTE_TABLE_OFFSET) + (videoX / 32) + (((scanlinesFineY - 240) / 32) * 8)];
		}
		else
		{
			attributeByte =
				this->memory[(nametableAddress + ATTRIBUTE_TABLE_OFFSET) + (videoX / 32) + ((scanlinesFineY / 32) * 8)];
		}
		int xQuadrant = videoX % 32;
		int yQuadrant;
		if (nametableAddress == mirrorNametableAddress)
		{
			yQuadrant = (scanlinesFineY - 240) % 32;
		}
		else
		{
			yQuadrant = scanlinesFineY % 32;
		}

		uint8_t paletteIndex = getPaletteIndex(xQuadrant, yQuadrant, attributeByte);
		
		uint8_t byteMask = 0x80;
		for (int bit = 0; bit < 8; bit++)
		{
			uint8_t firstPlaneBit = (firstPlaneByte & byteMask) ? 1 : 0;
			uint8_t secondPlaneBit = (secondPlaneByte & byteMask) ? 1 : 0;

			uint8_t pixelBits = (secondPlaneBit << 1) | firstPlaneBit;

			uint8_t pixelValue = this->memory[PALETTES_ADDRESS + pixelBits + (4 * paletteIndex)];
			if (pixelBits == 0)
			{
				pixelValue = this->memory[PALETTES_ADDRESS];
			}
			int pixelColor = getPixelColor(pixelValue);
			if (scanlines == 225 && fineY != 0 && pixelValue != 0x0f)
			{
				int a = 2;
			}
			setPixel(videoX + bit, scanlines, pixelColor);

			byteMask >>= 1;
		}
		nametableIndex++;
	}
	scanlines++;
}

void PPU::renderOAM() {
	for (int oamByte = 252; oamByte >= 0; oamByte -= 4)
	{
		uint8_t yPosition  = oam[oamByte];
		uint8_t tileIndex  = oam[oamByte + 1];
		uint8_t attributes = oam[oamByte + 2];
		uint8_t xPosition  = oam[oamByte + 3];

		uint8_t paletteIndex = attributes & 0x3;
		renderSprite(tileIndex, xPosition, yPosition, paletteIndex);
	}
}

void PPU::setPixel(int x, int y, int pixelColor) {
	video[x + (y * VIDEO_WIDTH)] = pixelColor;
}

void PPU::renderSprite(int spriteIndex, int videoX, int videoY, int paletteIndex) {
	uint16_t addressSprite = (spriteIndex * 16) + spritePatternTableAddress;
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
			if (pixelBits != 0)
			{
				uint8_t pixelValue = this->memory[PALETTES_ADDRESS + pixelBits + (4 * paletteIndex) + 0x10];
				int pixelColor = getPixelColor(pixelValue);

				// Here "bit" works as an X offset and "byte" works as a Y offset for the sprite coordinates
				setPixel((videoX + bit), (videoY + byte), pixelColor);
			}

			byteMask >>= 1;
		}
	}
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


uint8_t PPU::getPaletteIndex(int xQuadrant, int yQuadrant, uint8_t attributeByte) {
	/*
	Attribute Byte = 76543210
	 _ _ _ _ _ _
	|     |     |
	| 10  | 32  |
	|_ _ _|_ _ _|
	|     |     |
	| 54  | 76  |
	|_ _ _|_ _ _|

	*/

	uint8_t paletteIndex = 0;

	if (xQuadrant < 16)
	{
		// Top left pixel
		if (yQuadrant < 16)
		{
			paletteIndex = (attributeByte) & 0x3;
		}
		// Bottom left pixel
		else
		{
			paletteIndex = (attributeByte >> 4) & 0x3;
		}
	}
	else
	{
		// Top right pixel
		if (yQuadrant < 16)
		{
			paletteIndex = (attributeByte >> 2) & 0x3;
		}
		// Bottom right pixel
		else
		{
			paletteIndex = (attributeByte >> 6) & 0x3;
		}
	}
	return paletteIndex;
}

int PPU::getPixelColor(int pixelValue) {
	int pixelColor = 0;
	if (isGrayscale)
	{
		pixelColor = colorsMapTable[pixelValue] & 0x30;
	}
	else
	{
		pixelColor = colorsMapTable[pixelValue];
	}

	if (isRedEmphasized)
	{
		pixelColor = emphasizeRed(pixelColor);
	}
	if (isGreenEmphasized)
	{
		pixelColor = emphasizeGreen(pixelColor);
	}
	if (isBlueEmphasized)
	{
		pixelColor = emphasizeBlue(pixelColor);
	}

	return pixelColor;
}

// Dim every color but red by 12.5%
int PPU::emphasizeRed(int color) {
	// Dim green
	color = (color & 0xFF00FF) | ((int)(color * 0.875) & 0x00FF00);
	// Dim blue
	color = (color & 0xFFFF00) | ((int)(color * 0.875) & 0x0000FF);

	return color;
}

// Dim every color but green by 12.5%
int PPU::emphasizeGreen(int color) {
	// Dim red
	color = (color & 0x00FFFF) | ((int)(color * 0.875) & 0xFF0000);
	// Dim blue
	color = (color & 0xFFFF00) | ((int)(color * 0.875) & 0x0000FF);

	return color;
}

// Dim every color but blue by 12.5%
int PPU::emphasizeBlue(int color) {
	// Dim red
	color = (color & 0x00FFFF) | ((int)(color * 0.875) & 0xFF0000);
	// Dim green
	color = (color & 0xFF00FF) | ((int)(color * 0.875) & 0x00FF00);

	return color;
}

int PPU::getNametableAddress() {
	if (fineY + scanlines >= 240)
	{
		return mirrorNametableAddress;
	}
	return baseNametableAddress;
}