#include <fstream>
#include <stdint.h>
#include <string>
#include <vector>
#include "../CPU/CPU.h"
#include "PPU.h"
#include "../Cartrige/Cartridge.h"
#include "../Graphics/Graphics.h"
#include "../Mappers/Mapper.h"
#include "../Mappers/Mapper003.h"
#include "../Input/Input.h"
#include "../Audio/Audio.h"
#include "../APU/APU.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <assert.h>

PPU::PPU() : 
	video(VIDEO_WIDTH * VIDEO_HEIGHT),
	backgroundPixelBits(VIDEO_WIDTH * VIDEO_HEIGHT),
	memory(0x3FFF + 1),
	oam(256),
	chr(0xFFFF + 1)
{
	//cycles = 21;
	isHighByte = true;
}

PPU::~PPU() {}

void PPU::loadPatternTable() {
	int videoX = 0;
	int videoY = 0;

	for (int spriteIndex = 0; spriteIndex < 0x1FF; spriteIndex++)
	{
		renderSprite(spriteIndex, &videoX, &videoY);
	}
}

void PPU::renderFrame() {
	if (enableSprites)
	{
		renderOAM();
	}
	Graphics::update(this->video.data(), (sizeof(this->video[0]) * VIDEO_WIDTH));
}

void PPU::renderOAM() {
	for (int oamByte = 252; oamByte >= 4; oamByte -= 4)
	{
		uint16_t yPosition  = oam.at(oamByte) + 1; // 16 bits in order to prevent overflowing from 0xFF to 0x00
		if (yPosition < 240)
		{
			uint8_t tileIndex = oam.at(oamByte + 1);
			uint8_t attributes = oam.at(oamByte + 2);
			uint8_t xPosition = oam.at(oamByte + 3);

			uint8_t paletteIndex = attributes & 0x3;
			bool isBehindBackground = attributes & 0x20;
			bool hFlip = attributes & 0x40;
			bool vFlip = attributes & 0x80;
			renderSprite(tileIndex, xPosition, yPosition, paletteIndex, vFlip, hFlip, isBehindBackground);
		}
	}
}

void PPU::setPixel(int x, int y, int pixelColor) {
	video.at(x + (y * VIDEO_WIDTH)) = pixelColor;
}
int PPU::getPixel(int x, int y) {
	return video.at(x + (y * VIDEO_WIDTH));
}

void PPU::renderSprite(int spriteIndex, int x, int y, int paletteIndex, bool vFlip, bool hFlip, bool isBehindBackground) {
	uint16_t addressSprite;
	if (spriteHeight == 8) // 8x8
	{
		addressSprite = (spriteIndex * 16) + spritePatternTableAddress;
	}
	else // 8x16
	{
		if (spriteIndex & 0x1)
		{
			addressSprite = ((spriteIndex - 1) * 16) + 0x1000;
		}
		else
		{
			addressSprite = (spriteIndex * 16);
		}
	}

	for (int byte = 0; byte < spriteHeight; byte++)
	{
		if (y + byte >= VIDEO_HEIGHT) break;

		int yOffset = vFlip ? (spriteHeight - 1) - byte : byte;
		if (yOffset >= 8)
		{
			yOffset += 8;
		}
		uint8_t firstPlaneByte = this->memoryRead(addressSprite + yOffset);
		uint8_t secondPlaneByte = this->memoryRead(addressSprite + yOffset + 8);

		uint8_t byteMask = hFlip ? 0x1 : 0x80;
		int bit = 0;
		if (x < 8 && !showLeftmostSprites)
		{
			bit = 8 - x;
		}
		for (; bit < 8; bit++)
		{
			if (x + bit >= VIDEO_WIDTH) break;

			uint8_t firstPlaneBit = (firstPlaneByte & byteMask) ? 1 : 0;
			uint8_t secondPlaneBit = (secondPlaneByte & byteMask) ? 1 : 0;

			uint8_t pixelBits = (secondPlaneBit << 1) | firstPlaneBit;
			if (pixelBits != 0)
			{
				uint8_t pixelValue = this->memory.at(PALETTES_ADDRESS + pixelBits + (4 * paletteIndex) + 0x10);
				int pixelColor = getPixelColor(pixelValue);
				if (isBehindBackground)
				{
					if (backgroundPixelBits.at(x + bit + (VIDEO_WIDTH * (y + byte))) == 0)
					{
						// Here "bit" works as an X offset and "byte" works as a Y offset for the sprite coordinates
						setPixel((x + bit), (y + byte), pixelColor);
					}
				}
				else
				{
					// Here "bit" works as an X offset and "byte" works as a Y offset for the sprite coordinates
					setPixel((x + bit), (y + byte), pixelColor);
				}
			}
			byteMask = hFlip ? byteMask << 1 : byteMask >> 1;
		}
	}
}

bool PPU::renderSpritePixel(int spriteIndex, int x, int y, int paletteIndex, bool vFlip, bool hFlip, bool isBehindBackground) {
	bool hitDetected = false;
	uint16_t addressSprite;
	if (spriteHeight == 8) // 8x8
	{
		addressSprite = (spriteIndex * 16) + spritePatternTableAddress;
	}
	else // 8x16
	{
		if (spriteIndex & 0x1)
		{
			addressSprite = ((spriteIndex - 1) * 16) + 0x1000;
		}
		else
		{
			addressSprite = (spriteIndex * 16);
		}
	}

	int byte = scanlines - y;
	if (y + byte >= VIDEO_HEIGHT) return hitDetected;

	int yOffset = vFlip ? (spriteHeight - 1) - byte : byte;
	if (yOffset >= 8)
	{
		yOffset += 8;
	}
	uint8_t firstPlaneByte = this->memoryRead(addressSprite + yOffset);
	uint8_t secondPlaneByte = this->memoryRead(addressSprite + yOffset + 8);

	uint8_t byteMask = hFlip ? 0x1 : 0x80;

	int bit = (dot - 1) - x;
	byteMask = hFlip ? byteMask << bit : byteMask >> bit;
	if (x + bit >= VIDEO_WIDTH) return hitDetected;

	uint8_t firstPlaneBit = (firstPlaneByte & byteMask) ? 1 : 0;
	uint8_t secondPlaneBit = (secondPlaneByte & byteMask) ? 1 : 0;

	uint8_t pixelBits = (secondPlaneBit << 1) | firstPlaneBit;
	if (pixelBits != 0)
	{
		uint8_t pixelValue = this->memory.at(PALETTES_ADDRESS + pixelBits + (4 * paletteIndex) + 0x10);
		int pixelColor = getPixelColor(pixelValue);
		if (!hitDetected)
		{
			hitDetected = backgroundPixelBits.at(x + bit + (VIDEO_WIDTH * scanlines)) != 0;
		}
		if (!isBehindBackground || !hitDetected && enableSprites)
		{
			// Here "bit" works as an X offset and "byte" works as a Y offset for the sprite coordinates
			setPixel((x + bit), scanlines, pixelColor);
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

void PPU::incrementCoarseX() {
	// Coarse X == 31
	if ((vRegister & 0x1F) == 31)
	{
		// Coarse X = 0
		vRegister &= ~0x1F;
		// Switch horizontal nametable
		vRegister ^= 0x400;
	}
	else
	{
		vRegister++;
	}
}

void PPU::incrementY() {
	// Fine Y < 7
	if ((vRegister & 0x7000) != 0x7000)
	{
		vRegister += 0x1000;
	}
	else
	{
		// Fine Y = 0
		vRegister &= ~0x7000;
		int coarseY = (vRegister & 0x3E0) >> 5;
		if (coarseY == 29)
		{
			coarseY = 0;
			// Switch vertical nametable
			vRegister ^= 0x800;
		}
		else if (coarseY == 31)
		{
			coarseY = 0;
		}
		else
		{
			coarseY++;
		}
		vRegister = (vRegister & ~0x03E0) | (coarseY << 5);
	}
}

void PPU::step(CPU* cpu, bool updateFrame) {

	if (dot == 256 && (enableBackground || enableSprites) && renderState != POST_RENDER)
	{
		incrementY();
	}

	// Copy all bits related to horizontal position from t to v
	if (dot == 257 && (enableBackground || enableSprites) && renderState != POST_RENDER)
	{
		vRegister &= 0x7BE0;
		vRegister |= tRegister & 0x41F;
	}

	switch (this->renderState)
	{
	case PRE_RENDER:
		preRender();
		break;
	case RENDER:
		render();
		break;
	case POST_RENDER:
		postRender(cpu, updateFrame);
		break;
	}

	cycles++;
	dot++;
	if (dot == 341)
	{
		dot = 0;
		scanlines++;
		if (scanlines == 262)
		{
			scanlines = 0;
		}
		defineRenderState();
	}
}

void PPU::preRender() {
	if (dot == 339 && isOddFrame && (enableBackground || enableSprites))
	{
		dot = 0;
		scanlines = 0;
		this->renderState = RenderState::RENDER;
		return;
	}

	if (dot == 1)
	{
		this->backgroundIndex = 0;
		// Clear Sprite 0, VBlank and Sprite overflow
		this->regPpuStatus &= ~0xE0;
	}

	else if (dot >= 280 && dot <= 304 && (enableBackground || enableSprites))
	{
		// Repeatedly copy the vertical bits from t to v
		vRegister &= 0x41F;
		vRegister |= tRegister & 0x7BE0;
	}
}

void PPU::render() {
	if (dot == 0)
	{
		// Load first tile's data into shift registers

		uint16_t tileAddress = 0x2000 | (vRegister & 0xFFF);
		uint8_t tileIndex = this->memoryRead(tileAddress);

		uint16_t addressSprite = backgroundPatternTableAddress + tileIndex * 16;

		int fineY = (vRegister & 0x7000) >> 12;
		uint8_t currentFirstPlaneByte = this->memoryRead(addressSprite + fineY);
		uint8_t currentSecondPlaneByte = this->memoryRead(addressSprite + fineY + 8);

		firstPlaneByteData = secondPlaneByteData = 0;
		firstPlaneByteData = currentFirstPlaneByte << 8;
		secondPlaneByteData = currentSecondPlaneByte << 8;

		uint16_t attributeByteAddr = 0x23C0 | (vRegister & 0x0C00) | ((vRegister >> 4) & 0x38) | ((vRegister >> 2) & 0x07);
		this->currentAttributeByte = this->memoryRead(attributeByteAddr);
	}

	if (dot > 0 && dot <= 256 && enableBackground)
	{
		int x = dot - 1;
		int y = scanlines;

		// Tile is changed
		if (((x + xRegister) & 0x7) == 0)
		{
			// Update attribute byte

			this->coarseX = vRegister & 0x1F;
			this->coarseY = (vRegister & 0x3E0) >> 5;
			uint16_t attributeByteAddr = 0x23C0 | (vRegister & 0x0C00) | ((vRegister >> 4) & 0x38) | ((vRegister >> 2) & 0x07);
			this->currentAttributeByte = this->memoryRead(attributeByteAddr);
		}

		if ((x & 0x7) == 0)
		{
			loadShiftRegisters();
		}

		int paletteIndex = getPaletteIndex((coarseX * 8) % 32, (coarseY * 8) % 32, this->currentAttributeByte);

		uint16_t byteMask = 0x8000 >> xRegister;
		uint8_t firstPlaneBit = (firstPlaneByteData & byteMask) ? 1 : 0;
		uint8_t secondPlaneBit = (secondPlaneByteData & byteMask) ? 1 : 0;

		firstPlaneByteData <<= 1;
		secondPlaneByteData <<= 1;

		uint8_t pixelBits = (secondPlaneBit << 1) | firstPlaneBit;

		uint8_t pixelValue;
		if (pixelBits == 0 || (x < 8 && !showLeftmostBackground))
		{
			pixelValue = this->memory.at(PALETTES_ADDRESS);
		}
		else
		{
			pixelValue = this->memory.at(PALETTES_ADDRESS + pixelBits + (4 * paletteIndex));
		}

		int pixelColor = getPixelColor(pixelValue);
		setPixel(x, y, pixelColor);
		backgroundPixelBits.at(backgroundIndex) = pixelBits;
		backgroundIndex++;

		if (enableSprites)
		{
			handleSpriteZero();
		}
	}
}


void PPU::postRender(CPU* cpu, bool updateFrame) {
	if (scanlines == 241 && dot == 1)
	{
		if (updateFrame)
		{
			this->renderFrame();
		}
		Input::inputProcessing(&cpu->controller);

		// Set VBlank flag
		this->regPpuStatus |= 0x80;
		if (this->regPpuCtrl & 0x80)
		{
			cpu->nmiInterrupt = true;
		}

		isOddFrame = !isOddFrame;
	}
}

void PPU::defineRenderState() {
	if (scanlines < 240)
	{
		this->renderState = RenderState::RENDER;
	}
	else if (scanlines < 261)
	{
		this->renderState = RenderState::POST_RENDER;
	}
	else
	{
		this->renderState = RenderState::PRE_RENDER;
	}
}

void PPU::loadShiftRegisters() {
	incrementCoarseX();
	uint16_t tileAddress = 0x2000 | (vRegister & 0xFFF);

	uint8_t tileIndex = this->memoryRead(tileAddress);

	uint16_t addressSprite = backgroundPatternTableAddress + tileIndex * 16;

	int fineY = (vRegister & 0x7000) >> 12;
	uint8_t nextFirstPlaneByte = this->memoryRead(addressSprite + fineY);
	uint8_t nextSecondPlaneByte = this->memoryRead(addressSprite + fineY + 8);

	this->firstPlaneByteData |= nextFirstPlaneByte;
	this->secondPlaneByteData |= nextSecondPlaneByte;
}

void PPU::handleSpriteZero() {
	uint16_t yPosition = oam[0] + 1; // 16 bits in order to prevent overflowing from 0xFF to 0x00
	uint8_t xPosition = oam[3];
	if (scanlines >= yPosition && scanlines - yPosition < spriteHeight && ((dot - 1) - xPosition < 8) && (dot - 1) >= xPosition)
	{
		uint8_t tileIndex = oam[1];
		uint8_t attributes = oam[2];

		uint8_t paletteIndex = attributes & 0x3;
		bool isBehindBackground = attributes & 0x20;
		bool vFlip = attributes & 0x80;
		bool hFlip = attributes & 0x40;
		if (renderSpritePixel(tileIndex, xPosition, yPosition, paletteIndex, vFlip, hFlip, isBehindBackground))
		{
			// Set sprite 0 hit flag
			regPpuStatus |= 0x40;
		}
	}
}
