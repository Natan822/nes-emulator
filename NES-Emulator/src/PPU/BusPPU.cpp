#include "PPU.h"
#include "../CPU/CPU.h"
#include "../Mappers/Mapper.h"
#include "../Mappers/Mapper003.h"
#include <iostream>

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
		this->oam.at(cpu->memory.at(OAMADDR)) = data;
		// Increment OAMADDR
		cpu->memory.at(OAMADDR)++;
		regOamAddr++;
		break;

	case PPUSCROLL:
		regPpuScroll = data;
		updatePPUSCROLL();
		break;

	case PPUADDR:
		if (isHighByte)
		{
			tRegister &= ~0x7F00;
			tRegister |= (data & 0x3F) << 8;
		}
		else
		{
			tRegister &= ~0x00FF;
			tRegister |= data;
			vRegister = tRegister;
		}
		isHighByte = !isHighByte;
		if (vRegister > 0x7FFF)
		{
			vRegister &= 0x7FFF;
		}

		
		regPpuAddr = data;
		break;
	case PPUDATA:
		if (vRegister >= PALETTES_ADDRESS)
		{
			writePalettes(data);
		}
		else
		{
			memoryWrite(vRegister, data);
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
			this->oam[byteIndex] = cpu->memory.at(sourceAddress + byteIndex);
		}
		cpu->cycles += 513;
		regOamDma = data;
		break;
	}
	default:
		break;
	}

	cpu->memory.at(address) = data;
	return data;
}

uint8_t PPU::readMemoryPpu(uint16_t address, CPU* cpu) {
	uint8_t data = cpu->memory.at(address);
	switch (address)
	{
	case PPUCTRL:
		break;

	case PPUMASK:
		break;

	case PPUSTATUS:
		data = regPpuStatus;
		// Clear vblank_flag on read
		cpu->memory.at(address) &= ~0x80;
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
		data = readBuffer;
		if (vRegister >= PALETTES_ADDRESS)
		{
			readBuffer = readPalettes();
		}
		else
		{
			readBuffer = memoryRead(vRegister);
		}
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

uint8_t PPU::memoryRead(uint16_t address)
{
	// CHR
	if (address < 0x2000)
	{
		return this->mapper->chrRead(address);
	}
	// Nametables
	if (address < 0x3000)
	{
		switch (mirrorType)
		{
		case PPU::HORIZONTAL:
			// Nametable 2
			if (address >= 0x2400 && address < 0x2800)
			{
				return this->memory.at(address & 0x23FF); // Mirror of Nametable 1
			}
			// Nametable 4
			if (address >= 0x2C00)
			{
				return this->memory.at(address & 0x2BFF); // Mirror of Nametable 3
			}
			// Nametables 1 and 3
			return this->memory.at(address);

		case PPU::VERTICAL:
			// Nametable 3
			if (address >= 0x2800 && address < 0x2C00)
			{
				return this->memory.at(address & 0x27FF); // Mirror of Nametable 1
			}
			// Nametable 4
			if (address >= 0x2C00)
			{
				return this->memory.at(address & 0x27FF); // Mirror of Nametable 2
			}
			// Nametables 1 and 2
			return this->memory.at(address);
		}
	}
	return this->memory.at(address);
}

void PPU::memoryWrite(uint16_t address, uint8_t data)
{
	if (address < 0x2000)
	{
		this->mapper->chrWrite(address, data);
	}
	// Nametables
	else if (address < 0x3000)
	{
		switch (mirrorType)
		{
		case PPU::HORIZONTAL:
			// Nametable 2
			if (address >= 0x2400 && address < 0x2800)
			{
				this->memory.at(address & 0x23FF) = data; // Mirror of Nametable 1
			}
			// Nametable 4
			else if (address >= 0x2C00)
			{
				this->memory.at(address & 0x2BFF) = data; // Mirror of Nametable 3
			}
			// Nametables 1 and 3
			else
			{
				this->memory.at(address) = data;
			}
			break;

		case PPU::VERTICAL:
			// Nametable 3
			if (address >= 0x2800 && address < 0x2C00)
			{
				this->memory.at(address & 0x27FF) = data; // Mirror of Nametable 1
			}
			// Nametable 4
			else if (address >= 0x2C00)
			{
				this->memory.at(address & 0x27FF) = data; // Mirror of Nametable 2
			}
			// Nametables 1 and 2
			else
			{
				this->memory.at(address) = data;
			}
			break;
		}
	}
	else
	{
		this->memory.at(address) = data;
	}
}

void PPU::updatePPUCTRL() {
	changeBaseNametable(regPpuCtrl & 0x3);

	// Update nametable selection
	tRegister &= ~0xC00;
	tRegister |= ((regPpuCtrl & 0x3) << 10);

	// Check sprite pattern table address
	spritePatternTableAddress = regPpuCtrl & 0x8 ? 0x1000 : 0;

	// Check background pattern table address
	backgroundPatternTableAddress = regPpuCtrl & 0x10 ? 0x1000 : 0;
}

void PPU::updatePPUMASK() {
	isGrayscale = regPpuMask & 0x1;
	showLeftmostBackground = regPpuMask & 0x2;
	showLeftmostSprites = regPpuMask & 0x4;
	enableBackground = regPpuMask & 0x8;
	enableSprites = regPpuMask & 0x10;
	isRedEmphasized = regPpuMask & 0x20;
	isGreenEmphasized = regPpuMask & 0x40;
	isBlueEmphasized = regPpuMask & 0x80;
}

void PPU::updatePPUSCROLL() {
	// First write
	if (isHighByte)
	{
		scrollX = regPpuScroll;

		tRegister &= ~0x1F;
		tRegister |= ((regPpuScroll & 0xF8) >> 3);
		xRegister = regPpuScroll & 0x7;
	}
	// Second write
	else
	{
		scrollY = regPpuScroll;

		tRegister &= ~0x73E0;
		tRegister |= (regPpuScroll & 0x3) << 12;
		tRegister |= (regPpuScroll & 0xF8) << 2;
	}
	isHighByte = !isHighByte;
}

void PPU::vramIncrease(CPU* cpu) {
	// Check VRAM address increment mode
	if (regPpuCtrl & 0x4)
	{
		vRegister += 32;
	}
	else
	{
		vRegister++;
	}
	if (vRegister > 0x7FFF)
	{
		vRegister &= 0x7FFF;
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
		this->memory.at(0x3F20 + i) = this->memory.at(PALETTES_ADDRESS + paletteIndex);
		paletteIndex++;
	}
}

void PPU::writePalettes(uint8_t data) {
	if (vRegister <= 0x3F1F)
	{
		uint8_t lastDigit = vRegister & 0xF;
		// Backdrop color
		if (lastDigit % 4 == 0)
		{
			this->memory[0x3F00 | lastDigit] = data;
			this->memory[0x3F10 | lastDigit] = data;
		}
		else
		{
			this->memory[vRegister] = data;
		}
	}
	else
	{
		uint8_t mirroredByte = (vRegister & 0xFF) % 0x20;
		this->memory[PALETTES_ADDRESS | mirroredByte] = data;
	}
}

uint8_t PPU::readPalettes() {
	if (vRegister <= 0x3F1F)
	{
		// Backdrop color
		if ((vRegister & 0xFF) % 4 == 0)
		{
			return this->memory[PALETTES_ADDRESS];
		}

		return this->memory[vRegister];
	}

	uint8_t mirroredByte = (vRegister & 0xFF) % 0x20;
	return this->memory[(vRegister & 0x3F00) | mirroredByte];
}