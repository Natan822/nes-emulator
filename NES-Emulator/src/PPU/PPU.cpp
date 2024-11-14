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

	mirrorType = buffer[6] & 0x1 ? VERTICAL : HORIZONTAL;

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
	if (enableSprites)
	{
		renderOAM();
	}
	Graphics::update(this->video, (sizeof(this->video[0]) * VIDEO_WIDTH));

	cpu->memory[PPUSTATUS] |= 0x80;
	regPpuStatus |= 0x80;
	if (regPpuCtrl & 0x80)
	{
		cpu->nmiInterrupt = true;
	}
}

void PPU::renderScanline() {

	if (mirrorType == VERTICAL)
	{
		renderScanlineVerticalMirroring();
	}
	else
	{
		renderScanlineHorizontalMirroring();
	}

	// Sprite 0 hit check
	uint8_t yPosition = oam[0];
	if (scanlines == yPosition + 8 && enableSprites)
	{
		uint8_t tileIndex = oam[1];
		uint8_t attributes = oam[2];
		uint8_t xPosition = oam[3];

		uint8_t paletteIndex = attributes & 0x3;
		bool vFlip = attributes & 0x80;
		bool hFlip = attributes & 0x40;
		if (renderSpriteHitDetect(tileIndex, xPosition, yPosition, paletteIndex, vFlip, hFlip))
		{
			// Set sprite 0 hit flag
			regPpuStatus |= 0x40;
		}
	}

}

void PPU::renderOAM() {
	for (int oamByte = 252; oamByte >= 4; oamByte -= 4)
	{
		uint8_t yPosition  = oam[oamByte];
		uint8_t tileIndex  = oam[oamByte + 1];
		uint8_t attributes = oam[oamByte + 2];
		uint8_t xPosition  = oam[oamByte + 3];

		uint8_t paletteIndex = attributes & 0x3;
		bool hFlip = attributes & 0x40;
		bool vFlip = attributes & 0x80;
		renderSprite(tileIndex, xPosition, yPosition, paletteIndex, vFlip, hFlip);
	}
}

void PPU::setPixel(int x, int y, int pixelColor) {
	video[x + (y * VIDEO_WIDTH)] = pixelColor;
}
int PPU::getPixel(int x, int y) {
	return video[x + (y * VIDEO_WIDTH)];
}

void PPU::renderSprite(int spriteIndex, int x, int y, int paletteIndex, bool vFlip, bool hFlip) {
	uint16_t addressSprite = (spriteIndex * 16) + spritePatternTableAddress;
	for (int byte = 0; byte < 8; byte++)
	{
		uint8_t firstPlaneByte = this->memory[addressSprite + byte];
		uint8_t secondPlaneByte = this->memory[addressSprite + byte + 8];

		uint8_t byteMask = hFlip ? 0x1 : 0x80;
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
				setPixel((x + bit), (y + byte), pixelColor);
			}
			byteMask = hFlip ? byteMask << 1 : byteMask >> 1;
		}
	}
}

bool PPU::renderSpriteHitDetect(int spriteIndex, int x, int y, int paletteIndex, bool vFlip, bool hFlip) {
	bool hitDetected = false;

	uint16_t addressSprite = (spriteIndex * 16) + spritePatternTableAddress;

	for (int byte = 0; byte < 8; byte++)
	{
		uint8_t firstPlaneByte = this->memory[addressSprite + byte];
		uint8_t secondPlaneByte = this->memory[addressSprite + byte + 8];

		uint8_t byteMask = hFlip ? 0x1 : 0x80;

		for (int bit = 0; bit < 8; bit++)
		{
			uint8_t firstPlaneBit = (firstPlaneByte & byteMask) ? 1 : 0;
			uint8_t secondPlaneBit = (secondPlaneByte & byteMask) ? 1 : 0;

			uint8_t pixelBits = (secondPlaneBit << 1) | firstPlaneBit;
			if (pixelBits != 0)
			{
				uint8_t pixelValue = this->memory[PALETTES_ADDRESS + pixelBits + (4 * paletteIndex) + 0x10];
				int pixelColor = getPixelColor(pixelValue);
				int backgroundPixel = getPixel((x + bit), (y + byte));
				for (int backdropColor = 0; backdropColor < 4; backdropColor++)
				{
					if (backgroundPixel == getPixelColor(this->memory[PALETTES_ADDRESS + backdropColor]))
					{
						hitDetected = true;
						break;
					}
				}
				// Here "bit" works as an X offset and "byte" works as a Y offset for the sprite coordinates
				setPixel((x + bit), (y + byte), pixelColor);
			}
			byteMask = hFlip ? byteMask << 1 : byteMask >> 1;
		}
	}
	return hitDetected;
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
			video[*videoX + bit + ((*videoY + byte) * VIDEO_WIDTH)] = Graphics::colorsMapTable[pixelValue];

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
		pixelColor = Graphics::colorsMapTable[pixelValue] & 0x30;
	}
	else
	{
		pixelColor = Graphics::colorsMapTable[pixelValue];
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
	color = (color & 0xFF00FF) | ((int)((color & 0x00FF00) * 0.875) & 0x00FF00);
	// Dim blue
	color = (color & 0xFFFF00) | ((int)((color & 0x0000FF) * 0.875) & 0x0000FF);

	return color;
}

// Dim every color but green by 12.5%
int PPU::emphasizeGreen(int color) {
	// Dim red
	color = (color & 0x00FFFF) | ((int)((color & 0xFF0000) * 0.875) & 0xFF0000);
	// Dim blue
	color = (color & 0xFFFF00) | ((int)((color & 0x0000FF) * 0.875) & 0x0000FF);

	return color;
}

// Dim every color but blue by 12.5%
int PPU::emphasizeBlue(int color) {
	// Dim red
	color = (color & 0x00FFFF) | ((int)((color & 0xFF0000) * 0.875) & 0xFF0000);
	// Dim green
	color = (color & 0xFF00FF) | ((int)((color & 0x00FF00) * 0.875) & 0x00FF00);

	return color;
}

int PPU::getNametableAddress() {
	if (fineY + scanlines >= 240)
	{
		return mirrorNametableAddress;
	}
	return baseNametableAddress;
}

void PPU::renderScanlineHorizontalMirroring() {
	int nametableIndex = 0;
	int nametableAddress = getNametableAddress();

	int scanlinesFineY = scanlines + fineY;
	for (int videoX = 0; videoX < VIDEO_WIDTH; videoX += 8)
	{
		uint8_t tileIndex;
		uint8_t attributeByte;
		int yQuadrant;
		if (nametableAddress == mirrorNametableAddress)
		{
			tileIndex =
				this->memory[nametableAddress + nametableIndex + (((scanlinesFineY - 240) / 8) * 32)];

			attributeByte =
				this->memory[(nametableAddress + ATTRIBUTE_TABLE_OFFSET) + (videoX / 32) + (((scanlinesFineY - 240) / 32) * 8)];

			yQuadrant = (scanlinesFineY - 240) % 32;
		}
		else
		{
			tileIndex =
				this->memory[nametableAddress + nametableIndex + ((scanlinesFineY / 8) * VIDEO_WIDTH / 8)];

			attributeByte =
				this->memory[(nametableAddress + ATTRIBUTE_TABLE_OFFSET) + (videoX / 32) + ((scanlinesFineY / 32) * 8)];

			yQuadrant = scanlinesFineY % 32;
		}
		uint16_t addressSprite = (tileIndex * 16) + backgroundPatternTableAddress;

		int videoY = scanlines % 8;

		uint8_t firstPlaneByte = this->memory[addressSprite + videoY];
		uint8_t secondPlaneByte = this->memory[addressSprite + videoY + 8];

		int xQuadrant = videoX % 32;

		uint8_t paletteIndex = getPaletteIndex(xQuadrant, yQuadrant, attributeByte);

		uint8_t byteMask = 0x80;
		for (int bit = 0; bit < 8; bit++)
		{
			uint8_t firstPlaneBit = (firstPlaneByte & byteMask) ? 1 : 0;
			uint8_t secondPlaneBit = (secondPlaneByte & byteMask) ? 1 : 0;

			uint8_t pixelBits = (secondPlaneBit << 1) | firstPlaneBit;

			uint8_t pixelValue;
			if (pixelBits == 0)
			{
				pixelValue = this->memory[PALETTES_ADDRESS];
			}
			else
			{
				pixelValue = this->memory[PALETTES_ADDRESS + pixelBits + (4 * paletteIndex)];
			}

			int pixelColor = getPixelColor(pixelValue);
			setPixel(videoX + bit, scanlines, pixelColor);

			byteMask >>= 1;
		}
		nametableIndex++;
	}
	scanlines++;
}

void PPU::renderScanlineVerticalMirroring() {
	int nametableAddress;

	for (int videoX = 0; videoX < VIDEO_WIDTH; videoX += 8)
	{
		if (videoX + fineX >= VIDEO_WIDTH)
		{
			nametableAddress = mirrorNametableAddress;
		}
		else
		{
			nametableAddress = baseNametableAddress;
		}

		uint8_t tileIndex;
		uint8_t attributeByte;
		int xQuadrant;
		if (nametableAddress == mirrorNametableAddress)
		{
			tileIndex = this->memory[nametableAddress + ((videoX + fineX) - VIDEO_WIDTH) / 8 + ((scanlines / 8) * 32)];
			attributeByte = 
				this->memory[(nametableAddress + ATTRIBUTE_TABLE_OFFSET) + ((videoX + fineX) - VIDEO_WIDTH) / 32 + ((scanlines / 32) * 8)];
			xQuadrant = (videoX + fineX - VIDEO_WIDTH) % 32;
		}
		else
		{
			tileIndex =
				this->memory[nametableAddress + ((videoX + fineX) / 8) + ((scanlines / 8) * VIDEO_WIDTH / 8)];

			attributeByte =
				this->memory[(nametableAddress + ATTRIBUTE_TABLE_OFFSET) + ((videoX + fineX) / 32) + ((scanlines / 32) * 8)];
			xQuadrant = (videoX + fineX) % 32;
		}

		uint16_t addressSprite = (tileIndex * 16) + backgroundPatternTableAddress;

		int videoY = scanlines % 8;

		uint8_t firstPlaneByte = this->memory[addressSprite + videoY];
		uint8_t secondPlaneByte = this->memory[addressSprite + videoY + 8];

		int yQuadrant = scanlines % 32;

		uint8_t paletteIndex = getPaletteIndex(xQuadrant, yQuadrant, attributeByte);

		uint8_t byteMask = 0x80;
		for (int bit = 0; bit < 8; bit++)
		{
			uint8_t firstPlaneBit = (firstPlaneByte & byteMask) ? 1 : 0;
			uint8_t secondPlaneBit = (secondPlaneByte & byteMask) ? 1 : 0;

			uint8_t pixelBits = (secondPlaneBit << 1) | firstPlaneBit;

			uint8_t pixelValue;
			if (pixelBits == 0)
			{
				pixelValue = this->memory[PALETTES_ADDRESS];
			}
			else
			{
				pixelValue = this->memory[PALETTES_ADDRESS + pixelBits + (4 * paletteIndex)];
			}

			int pixelColor = getPixelColor(pixelValue);
			setPixel(videoX + bit, scanlines, pixelColor);

			byteMask >>= 1;
		}
	}
	scanlines++;
}