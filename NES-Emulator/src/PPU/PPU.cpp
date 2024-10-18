#include <fstream>
#include <stdint.h>
#include <string>
#include "../CPU/CPU.h"
#include "PPU.h"

#include <iostream>

PPU::PPU(CPU& cpu) {
	ppuCtrl = &cpu.memory[0x2000];
	ppuMask = &cpu.memory[0x2001];
	ppuStatus = &cpu.memory[0x2002];
	oamAddr = &cpu.memory[0x2003];
	oamData = &cpu.memory[0x2004];
	ppuScroll = &cpu.memory[0x2005];
	ppuAddr = &cpu.memory[0x2006];
	ppuData = &cpu.memory[0x2007];
	oamDma = &cpu.memory[0x4014];
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
}

void PPU::loadPatternTable() {
	int videoX = 0;
	int videoY = 0;
	int spriteCount = 0;
	for (int spritePointer = 0; spritePointer < 0x1FFF; spritePointer += 0x10)
	{
		for (int byte = 0; byte < 8; byte++)
		{
			uint8_t firstPlaneByte = memory[spritePointer + byte];
			uint8_t secondPlaneByte = memory[spritePointer + byte + 8];

			uint8_t byteMask = 0x80;
			for (int bit = 0; bit < 8; bit++)
			{
				uint8_t firstPlaneBit = firstPlaneByte & byteMask;
				uint8_t secondPlaneBit = secondPlaneByte & byteMask;

				if (firstPlaneBit || secondPlaneBit)
				{
					// Here "bit" works as an X offset and "byte" works as a Y offset for the sprite coordinates
					video[videoX + bit + ((videoY + byte) * VIDEO_WIDTH)] = 0xFFFFFFFF;
				}
				else
				{
					video[videoX + bit + ((videoY + byte) * VIDEO_WIDTH)] = 0;
				}
				byteMask >>= 1;
			}
		}
		videoX += 8;
		if (videoX >= VIDEO_WIDTH)
		{
			videoX = 0;
			videoY += 8;
		}
		spriteCount++;
	}
}