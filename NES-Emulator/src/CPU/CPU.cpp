#include <fstream>
#include <stdint.h>
#include <string>

#include <iostream>

#include "CPU.h"
#include "../PPU/PPU.h"

CPU::CPU(PPU& ppu) : ppu(ppu) {
	sp = STACK_START_ADDRESS & 0xFF;

	for (int i = 0; i < 0xFF + 1; i++)
	{
		table[i] = &CPU::OP_EA;
	}

	status = 0x24;
	pc = 0xC000;

	cycles = 7;

	// Fill opcodes table
	table[0x00] = &CPU::OP_00NN;
	table[0x01] = &CPU::OP_01NN;
	table[0x05] = &CPU::OP_05NN;
	table[0x06] = &CPU::OP_06NN;
	table[0x08] = &CPU::OP_08;
	table[0x09] = &CPU::OP_09NN;
	table[0x0A] = &CPU::OP_0A;
	table[0x0D] = &CPU::OP_0DNN00;
	table[0x0E] = &CPU::OP_0ENN00;
	table[0x10] = &CPU::OP_10NN;
	table[0x11] = &CPU::OP_11NN;
	table[0x15] = &CPU::OP_15NN;
	table[0x16] = &CPU::OP_16NN;
	table[0x18] = &CPU::OP_18;
	table[0x19] = &CPU::OP_19NN00;
	table[0x1D] = &CPU::OP_1DNN00;
	table[0x1E] = &CPU::OP_1ENN00;
	table[0x20] = &CPU::OP_20NN00;
	table[0x21] = &CPU::OP_21NN;
	table[0x24] = &CPU::OP_24NN;
	table[0x25] = &CPU::OP_25NN;
	table[0x26] = &CPU::OP_26NN;
	table[0x28] = &CPU::OP_28;
	table[0x29] = &CPU::OP_29NN;
	table[0x2A] = &CPU::OP_2A;
	table[0x2C] = &CPU::OP_2CNN00;
	table[0x2D] = &CPU::OP_2DNN00;
	table[0x2E] = &CPU::OP_2ENN00;
	table[0x30] = &CPU::OP_30NN;
	table[0x31] = &CPU::OP_31NN;
	table[0x35] = &CPU::OP_35NN;
	table[0x36] = &CPU::OP_36NN;
	table[0x38] = &CPU::OP_38;
	table[0x39] = &CPU::OP_39NN00;
	table[0x3D] = &CPU::OP_3DNN00;
	table[0x3E] = &CPU::OP_3ENN00;
	table[0x40] = &CPU::OP_40;
	table[0x41] = &CPU::OP_41NN;
	table[0x45] = &CPU::OP_45NN;
	table[0x46] = &CPU::OP_46NN;
	table[0x48] = &CPU::OP_48;
	table[0x49] = &CPU::OP_49NN;
	table[0x4A] = &CPU::OP_4A;
	table[0x4C] = &CPU::OP_4CNN00;
	table[0x4D] = &CPU::OP_4DNN00;
	table[0x4E] = &CPU::OP_4ENN00;
	table[0x50] = &CPU::OP_50NN;
	table[0x51] = &CPU::OP_51NN;
	table[0x55] = &CPU::OP_55NN;
	table[0x56] = &CPU::OP_56NN;
	table[0x58] = &CPU::OP_58;
	table[0x59] = &CPU::OP_59NN00;
	table[0x5D] = &CPU::OP_5DNN00;
	table[0x5E] = &CPU::OP_5ENN00;
	table[0x60] = &CPU::OP_60;
	table[0x61] = &CPU::OP_61NN;
	table[0x65] = &CPU::OP_65NN;
	table[0x66] = &CPU::OP_66NN;
	table[0x68] = &CPU::OP_68;
	table[0x69] = &CPU::OP_69NN;
	table[0x6A] = &CPU::OP_6A;
	table[0x6C] = &CPU::OP_6CNN00;
	table[0x6D] = &CPU::OP_6DNN00;
	table[0x6E] = &CPU::OP_6ENN00;
	table[0x70] = &CPU::OP_70NN;
	table[0x71] = &CPU::OP_71NN;
	table[0x75] = &CPU::OP_75NN;
	table[0x76] = &CPU::OP_76NN;
	table[0x78] = &CPU::OP_78;
	table[0x79] = &CPU::OP_79NN00;
	table[0x7D] = &CPU::OP_7DNN00;
	table[0x7E] = &CPU::OP_7ENN00;
	table[0x81] = &CPU::OP_81NN;
	table[0x84] = &CPU::OP_84NN;
	table[0x85] = &CPU::OP_85NN;
	table[0x86] = &CPU::OP_86NN;
	table[0x88] = &CPU::OP_88;
	table[0x8A] = &CPU::OP_8A;
	table[0x8C] = &CPU::OP_8CNN00;
	table[0x8D] = &CPU::OP_8DNN00;
	table[0x8E] = &CPU::OP_8ENN00;
	table[0x90] = &CPU::OP_90NN;
	table[0x91] = &CPU::OP_91NN;
	table[0x94] = &CPU::OP_94NN;
	table[0x95] = &CPU::OP_95NN;
	table[0x96] = &CPU::OP_96NN;
	table[0x98] = &CPU::OP_98;
	table[0x99] = &CPU::OP_99NN00;
	table[0x9A] = &CPU::OP_9A;
	table[0x9D] = &CPU::OP_9DNN00;
	table[0xA0] = &CPU::OP_A0NN;
	table[0xA1] = &CPU::OP_A1NN;
	table[0xA2] = &CPU::OP_A2NN;
	table[0xA4] = &CPU::OP_A4NN;
	table[0xA5] = &CPU::OP_A5NN;
	table[0xA6] = &CPU::OP_A6NN;
	table[0xA8] = &CPU::OP_A8;
	table[0xA9] = &CPU::OP_A9NN;
	table[0xAA] = &CPU::OP_AA;
	table[0xAC] = &CPU::OP_ACNN00;
	table[0xAD] = &CPU::OP_ADNN00;
	table[0xAE] = &CPU::OP_AENN00;
	table[0xB0] = &CPU::OP_B0NN;
	table[0xB1] = &CPU::OP_B1NN;
	table[0xB4] = &CPU::OP_B4NN;
	table[0xB5] = &CPU::OP_B5NN;
	table[0xB6] = &CPU::OP_B6NN;
	table[0xB8] = &CPU::OP_B8;
	table[0xB9] = &CPU::OP_B9NN00;
	table[0xBA] = &CPU::OP_BA;
	table[0xBC] = &CPU::OP_BCNN00;
	table[0xBD] = &CPU::OP_BDNN00;
	table[0xBE] = &CPU::OP_BENN00;
	table[0xC0] = &CPU::OP_C0NN;
	table[0xC1] = &CPU::OP_C1NN;
	table[0xC4] = &CPU::OP_C4NN;
	table[0xC5] = &CPU::OP_C5NN;
	table[0xC6] = &CPU::OP_C6NN;
	table[0xC8] = &CPU::OP_C8;
	table[0xC9] = &CPU::OP_C9NN;
	table[0xCA] = &CPU::OP_CA;
	table[0xCC] = &CPU::OP_CCNN00;
	table[0xCD] = &CPU::OP_CDNN00;
	table[0xCE] = &CPU::OP_CENN00;
	table[0xD0] = &CPU::OP_D0NN;
	table[0xD1] = &CPU::OP_D1NN;
	table[0xD5] = &CPU::OP_D5NN;
	table[0xD6] = &CPU::OP_D6NN;
	table[0xD8] = &CPU::OP_D8;
	table[0xD9] = &CPU::OP_D9NN00;
	table[0xDD] = &CPU::OP_DDNN00;
	table[0xDE] = &CPU::OP_DENN00;
	table[0xE0] = &CPU::OP_E0NN;
	table[0xE1] = &CPU::OP_E1NN;
	table[0xE4] = &CPU::OP_E4NN;
	table[0xE5] = &CPU::OP_E5NN;
	table[0xE6] = &CPU::OP_E6NN;
	table[0xE8] = &CPU::OP_E8;
	table[0xE9] = &CPU::OP_E9NN;
	table[0xEA] = &CPU::OP_EA;
	table[0xEC] = &CPU::OP_ECNN00;
	table[0xED] = &CPU::OP_EDNN00;
	table[0xEE] = &CPU::OP_EENN00;
	table[0xF0] = &CPU::OP_F0NN;
	table[0xF1] = &CPU::OP_F1NN;
	table[0xF5] = &CPU::OP_F5NN;
	table[0xF6] = &CPU::OP_F6NN;
	table[0xF8] = &CPU::OP_F8;
	table[0xF9] = &CPU::OP_F9NN00;
	table[0xFD] = &CPU::OP_FDNN00;
	table[0xFE] = &CPU::OP_FENN00;
}

CPU::~CPU() {}

void CPU::loadROM(std::string filePath) {
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
	chrSize = static_cast<int>(static_cast<unsigned char>(buffer[5]));

	if (prgSize == 2)
	{
		for (int i = 0; i < 32768; i++)
		{
			memory[PRG_START_ADDRESS + i] = buffer[i + 16];
		}
	}
	else if (prgSize == 1)
	{
		for (int i = 0; i < 16384; i++)
		{
			memory[PRG_START_ADDRESS + i] = buffer[i + 16];
		}
		for (int i = 16384; i < 32768; i++)
		{
			memory[PRG_START_ADDRESS + i] = buffer[i + 16 - 16384];
		}
	}

	delete[] buffer;

	uint8_t lowByteStartAddress = memory[RESET_VECTOR_ADDRESS];
	uint8_t highByteStartAddress = memory[RESET_VECTOR_ADDRESS + 1];

	uint16_t startAddress = (highByteStartAddress << 8) | lowByteStartAddress;
	//pc = startAddress;
}

void CPU::cycle() {
	if (irqInterrupt && !getFlag('I'))
	{
		handleInterrupt('I');
		cycles += 7;
	}
	if (memory[PPUCTRL] & 0x80)
	{
		handleInterrupt('N');
		cycles += 7;
	}
	execute();
}

void CPU::execute() {
	printInfo();

	// Every instruction takes at least 2 cycles
	cycles += 2;

	(this->*table[memory[pc]])();
}

void CPU::reset() {
	aReg = 0;
	xReg = 0;
	yReg = 0;
	status = 0x20;
	pc = RESET_VECTOR_ADDRESS;
}

void CPU::handleInterrupt(char interruptType) {
	uint8_t lowBytePc = pc & 0xFF;
	uint8_t highBytePc = pc >> 8;

	push(highBytePc);
	push(lowBytePc);
	push(status);

	setFlag('I', 1);
	switch (interruptType)
	{
	// IRQ interrupt
	case 'I':
	{
		uint8_t lowByteVectorAddress = memory[IRQ_VECTOR_ADDRESS];
		uint8_t highByteVectorAddress = memory[IRQ_VECTOR_ADDRESS + 1];
		uint16_t vectorAddress = (highByteVectorAddress << 8) | lowByteVectorAddress;
		pc = vectorAddress;

		break;
	}
	// NMI interrupt
	case 'N':
	{
		uint8_t lowByteVectorAddress = memory[NMI_VECTOR_ADDRESS];
		uint8_t highByteVectorAddress = memory[NMI_VECTOR_ADDRESS + 1];
		uint16_t vectorAddress = (highByteVectorAddress << 8) | lowByteVectorAddress;
		pc = vectorAddress;

		break;
	}
	// BRK interrupt
	/*
		BRK is handled by OP_00NN()
	*/
	default:
		break;
	}
}

void CPU::printInfo() {
	std::cout << std::hex << pc << " " << static_cast<int>(memory[pc]) << std::endl;
	std::cout << "A:" << static_cast<int>(aReg);
	std::cout << " X:" << static_cast<int>(xReg);
	std::cout << " Y:" << static_cast<int>(yReg);
	std::cout << " P:" << static_cast<int>(status);
	std::cout << " SP:" <<  static_cast<int>(sp);
	std::cout << " CYC:" << std::dec << cycles << std::endl;
}

uint8_t CPU::writeMemory(uint16_t address, uint8_t data) {
	if ((address >= 0x2000 && address <= 0x2007) || address == 0x4014)
	{
		return ppu.writeMemoryPpu(address, data, this);
	}
	memory[address] = data;
	return data;
}

uint8_t CPU::readMemory(uint16_t address) {
	if ((address >= 0x2000 && address <= 0x2007) || address == 0x4014)
	{
		return ppu.readMemoryPpu(address, this);
	}
	return memory[address];
}

void CPU::push(uint8_t value) {
	memory[sp + STACK_BOTTOM_ADDRESS] = value;
	sp--;
}

uint8_t CPU::pop() {
	sp++;
	return memory[sp + STACK_BOTTOM_ADDRESS];
}

void CPU::setFlag(char flag, uint8_t value) {
	switch (flag)
	{
	case 'N':
		status &= ~0x80;
		if (value)
		{
			status |= 0x80;
		}
		break;

	case 'V':
		status &= ~0x40;
		if (value)
		{
			status |= 0x40;
		}
		break;

	case 'B':
		status &= ~0x10;
		if (value)
		{
			status |= 0x10;
		}
		break;

	case 'D':
		status &= ~0x8;
		if (value)
		{
			status |= 0x8;
		}
		break;

	case 'I':
		status &= ~0x4;
		if (value)
		{
			status |= 0x4;
		}
		break;

	case 'Z':
		status &= ~0x2;
		if (value)
		{
			status |= 0x2;
		}
		break;

	case 'C':
		status &= ~0x1;
		if (value)
		{
			status |= 0x1;
		}
		break;

	default:
		break;
	}
}

bool CPU::getFlag(char flag) {
	switch (flag)
	{
	case 'N':
		return status & 0x80;

	case 'V':
		return status & 0x40;

	case 'B':
		return status & 0x10;

	case 'D':
		return status & 0x8;

	case 'I':
		return status & 0x4;

	case 'Z':
		return status & 0x2;

	case 'C':
		return status & 0x1;

	default:
		break;
	}
	return NULL;
}

