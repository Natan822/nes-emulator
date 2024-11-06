#include "PPU.h"
#include "../CPU/CPU.h"

uint8_t PPU::writeMemoryPpu(uint16_t address, uint8_t data, CPU* cpu) {
	switch (address)
	{
	case PPUCTRL:
		regPpuCtrl = data;
		updatePPUCTRL();
		break;

	case PPUMASK:
		regPpuMask = data;
		updatePPUMASK();
		break;

	case PPUSTATUS:
		regPpuStatus = data;
		break;

	case OAMADDR:
		regOamAddr = data;
		break;

	case OAMDATA:
		// Write to OAM
		this->oam[cpu->memory[OAMADDR]] = data;
		// Increment OAMADDR
		cpu->memory[OAMADDR]++;
		regOamAddr++;
		break;

	case PPUSCROLL:
		regPpuScroll = data;
		updatePPUSCROLL();
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
		if (vramAddress > 0x3FFF)
		{
			vramAddress &= 0x3FFF;
		}
		isHighByte = !isHighByte;
		regPpuAddr = data;
		break;

	case PPUDATA:
		if (vramAddress >= PALETTES_ADDRESS)
		{
			writePalettes(data);
		}
		else
		{
			this->memory[vramAddress] = data;
		}
		vramIncrease(cpu);
		regPpuData = data;
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
		regOamDma = data;
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
		regPpuStatus &= ~0x80;
		isHighByte = true;
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
	{
		uint8_t temp = readBuffer;
		if (vramAddress >= PALETTES_ADDRESS)
		{
			readBuffer = readPalettes();
		}
		else
		{
			readBuffer = this->memory[vramAddress];
		}
		data = temp;
		vramIncrease(cpu);
		break;
	}
	case OAMDMA:
		break;

	default:
		break;
	}

	return data;
}

void PPU::updatePPUCTRL() {
	// Check base and mirror nametables addresses
	switch (regPpuCtrl & 0x3)
	{
	case 0:
		baseNametableAddress = NAMETABLE1_ADDRESS;
		if (mirrorType == VERTICAL)
		{
			mirrorNametableAddress = NAMETABLE3_ADDRESS;
		}
		break;
	case 1:
		baseNametableAddress = NAMETABLE2_ADDRESS;
		if (mirrorType == VERTICAL)
		{
			mirrorNametableAddress = NAMETABLE4_ADDRESS;
		}
		break;
	case 2:
		baseNametableAddress = NAMETABLE3_ADDRESS;
		if (mirrorType == VERTICAL)
		{
			mirrorNametableAddress = NAMETABLE1_ADDRESS;
		}
		break;
	case 3:
		baseNametableAddress = NAMETABLE4_ADDRESS;
		if (mirrorType == VERTICAL)
		{
			mirrorNametableAddress = NAMETABLE2_ADDRESS;
		}
		break;
	}

	// Check sprite pattern table address
	spritePatternTableAddress = regPpuCtrl & 0x8 ? 0x1000 : 0;
	// Check background pattern table address
	backgroundPatternTableAddress = regPpuCtrl & 0x10 ? 0x1000 : 0;
}

void PPU::updatePPUMASK() {
	isGrayscale = regPpuMask & 0x1;
	isRedEmphasized = regPpuMask & 0x20;
	isGreenEmphasized = regPpuMask & 0x40;
	isBlueEmphasized = regPpuMask & 0x80;
}

void PPU::updatePPUSCROLL() {
	// First write
	if (isHighByte)
	{
		coarseY = (coarseY & 0x7) | ((regPpuScroll & 0x3) << 2);
	}
	// Second write
	else
	{
		coarseY = (coarseY & 18) | ((regPpuScroll & 0xE0) >> 5);
	}
	fineY = ((regPpuScroll & 0x70) >> 4) + (coarseY * 8);
	isHighByte = !isHighByte;
}

void PPU::vramIncrease(CPU* cpu) {
	// Check VRAM address increment mode
	if (regPpuCtrl & 0x4)
	{
		vramAddress += 32;
	}
	else
	{
		vramAddress++;
	}
	if (vramAddress > 0x3FFF)
	{
		vramAddress &= 0x3FFF;
	}
}

void PPU::mirrorPalettes() {
	int paletteIndex = 0;
	for (int i = 0; i <= 0xDF; i++)
	{
		if (paletteIndex > 0x1F)
		{
			paletteIndex = 0;
		}
		this->memory[0x320 + i] = this->memory[PALETTES_ADDRESS + paletteIndex];
		paletteIndex++;
	}
}

void PPU::writePalettes(uint8_t data) {
	if (vramAddress <= 0x3F1F)
	{
		// Backdrop color
		if ((vramAddress & 0xF) % 4 == 0)
		{
			uint8_t lastDigit = vramAddress & 0xF;
			this->memory[0x3F00 | lastDigit] = data;
			this->memory[0x3F10 | lastDigit] = data;
		}
		else
		{
			this->memory[vramAddress] = data;
		}
	}
	else
	{
		uint8_t mirroredByte = (vramAddress & 0xFF) % 0x20;
		this->memory[PALETTES_ADDRESS | mirroredByte] = data;
	}
}

uint8_t PPU::readPalettes() {
	if (vramAddress <= 0x3F1F)
	{
		// Backdrop color
		if ((vramAddress & 0xFF) % 4 == 0)
		{
			return this->memory[PALETTES_ADDRESS];
		}

		return this->memory[vramAddress];
	}

	uint8_t mirroredByte = (vramAddress & 0xFF) % 0x20;
	return this->memory[(vramAddress & 0xFF00) | mirroredByte];
}