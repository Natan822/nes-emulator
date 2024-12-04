#include <fstream>
#include <stdint.h>
#include <string>
#include <vector>
#include <memory>

#include <iostream>

#include "CPU.h"
#include "../Controller/Controller.h"
#include "../PPU/PPU.h"
#include "../Mappers/Mapper.h"
#include "../Mappers/Mapper003.h"

CPU::CPU(PPU& ppu, Controller& controller) : 
	ppu(ppu), 
	controller(controller),
	memory(0xFFFF + 1)
{
	sp = STACK_START_ADDRESS & 0xFF;

	for (int i = 0; i < 0xFF + 1; i++)
	{
		instructionsTable[i] = Instruction{ &CPU::invalid, 0, "Unknown", IMMEDIATE };
	}

	status = 0x24;
	pc = 0xC000;

	//cycles = 7;

	instructionsTable[0x00] = Instruction{ &CPU::OP_00NN, 7, "BRK", IMPLIED };
	instructionsTable[0x01] = Instruction{ &CPU::OP_01NN, 6, "ORA ($nn,X)", INDIRECTX };
	instructionsTable[0x05] = Instruction{ &CPU::OP_05NN, 3, "ORA $nn", ZEROPAGE };
	instructionsTable[0x06] = Instruction{ &CPU::OP_06NN, 5, "ASL $nn", ZEROPAGE };
	instructionsTable[0x08] = Instruction{ &CPU::OP_08, 3, "PHP", IMPLIED };
	instructionsTable[0x09] = Instruction{ &CPU::OP_09NN, 2, "ORA #$nn", IMMEDIATE };
	instructionsTable[0x0A] = Instruction{ &CPU::OP_0A, 2, "ASL A", ACCUMULATOR };
	instructionsTable[0x0D] = Instruction{ &CPU::OP_0DNN00, 4, "ORA $nnnn", ABSOLUTE };
	instructionsTable[0x0E] = Instruction{ &CPU::OP_0ENN00, 6, "ASL $nnnn", ABSOLUTE };
	instructionsTable[0x10] = Instruction{ &CPU::OP_10NN, 2, "BPL $nnnn", RELATIVE };
	instructionsTable[0x11] = Instruction{ &CPU::OP_11NN, 5, "ORA ($nn),Y", INDIRECTY };
	instructionsTable[0x15] = Instruction{ &CPU::OP_15NN, 4, "ORA $nn,X", ZEROPAGEX };
	instructionsTable[0x16] = Instruction{ &CPU::OP_16NN, 6, "ASL $nn,X", ZEROPAGEX };
	instructionsTable[0x18] = Instruction{ &CPU::OP_18, 2, "CLC", IMPLIED };
	instructionsTable[0x19] = Instruction{ &CPU::OP_19NN00, 4, "ORA $nnnn,Y", ABSOLUTEY };
	instructionsTable[0x1D] = Instruction{ &CPU::OP_1DNN00, 4, "ORA $nnnn,X", ABSOLUTEX };
	instructionsTable[0x1E] = Instruction{ &CPU::OP_1ENN00, 7, "ASL $nnnn,X", ABSOLUTEX };
	instructionsTable[0x20] = Instruction{ &CPU::OP_20NN00, 6, "JSR $nnnn", ABSOLUTE };
	instructionsTable[0x21] = Instruction{ &CPU::OP_21NN, 6, "AND ($nn,X)", INDIRECTX };
	instructionsTable[0x24] = Instruction{ &CPU::OP_24NN, 3, "BIT $nn", ZEROPAGE };
	instructionsTable[0x25] = Instruction{ &CPU::OP_25NN, 3, "AND $nn", ZEROPAGE };
	instructionsTable[0x26] = Instruction{ &CPU::OP_26NN, 5, "ROL $nn", ZEROPAGE };
	instructionsTable[0x28] = Instruction{ &CPU::OP_28, 4, "PLP", IMPLIED };
	instructionsTable[0x29] = Instruction{ &CPU::OP_29NN, 2, "AND #$nn", IMMEDIATE };
	instructionsTable[0x2A] = Instruction{ &CPU::OP_2A, 2, "ROL A", ACCUMULATOR };
	instructionsTable[0x2C] = Instruction{ &CPU::OP_2CNN00, 4, "BIT $nnnn", ABSOLUTE };
	instructionsTable[0x2D] = Instruction{ &CPU::OP_2DNN00, 4, "AND $nnnn", ABSOLUTE };
	instructionsTable[0x2E] = Instruction{ &CPU::OP_2ENN00, 6, "ROL $nnnn", ABSOLUTE };
	instructionsTable[0x30] = Instruction{ &CPU::OP_30NN, 2, "BMI $nnnn", RELATIVE };
	instructionsTable[0x31] = Instruction{ &CPU::OP_31NN, 5, "AND ($nn),Y", INDIRECTY };
	instructionsTable[0x35] = Instruction{ &CPU::OP_35NN, 4, "AND $nn,X", ZEROPAGEX };
	instructionsTable[0x36] = Instruction{ &CPU::OP_36NN, 6, "ROL $nn,X", ZEROPAGEX };
	instructionsTable[0x38] = Instruction{ &CPU::OP_38, 2, "SEC", IMPLIED };
	instructionsTable[0x39] = Instruction{ &CPU::OP_39NN00, 4, "AND $nnnn,Y", ABSOLUTEY };
	instructionsTable[0x3D] = Instruction{ &CPU::OP_3DNN00, 4, "AND $nnnn,X", ABSOLUTEX };
	instructionsTable[0x3E] = Instruction{ &CPU::OP_3ENN00, 7, "ROL $nnnn,X", ABSOLUTEX };
	instructionsTable[0x40] = Instruction{ &CPU::OP_40, 6, "RTI", IMPLIED };
	instructionsTable[0x41] = Instruction{ &CPU::OP_41NN, 6, "EOR ($nn,X)", INDIRECTX };
	instructionsTable[0x45] = Instruction{ &CPU::OP_45NN, 3, "EOR $nn", ZEROPAGE };
	instructionsTable[0x46] = Instruction{ &CPU::OP_46NN, 5, "LSR $nn", ZEROPAGE };
	instructionsTable[0x48] = Instruction{ &CPU::OP_48, 3, "PHA", IMPLIED };
	instructionsTable[0x49] = Instruction{ &CPU::OP_49NN, 2, "EOR #$nn", IMMEDIATE };
	instructionsTable[0x4A] = Instruction{ &CPU::OP_4A, 2, "LSR A", ACCUMULATOR };
	instructionsTable[0x4C] = Instruction{ &CPU::OP_4CNN00, 3, "JMP $nnnn", ABSOLUTE };
	instructionsTable[0x4D] = Instruction{ &CPU::OP_4DNN00, 4, "EOR $nnnn", ABSOLUTE };
	instructionsTable[0x4E] = Instruction{ &CPU::OP_4ENN00, 6, "LSR $nnnn", ABSOLUTE };
	instructionsTable[0x50] = Instruction{ &CPU::OP_50NN, 2, "BVC $nnnn", RELATIVE };
	instructionsTable[0x51] = Instruction{ &CPU::OP_51NN, 5, "EOR ($nn),Y", INDIRECTY };
	instructionsTable[0x55] = Instruction{ &CPU::OP_55NN, 4, "EOR $nn,X", ZEROPAGEX };
	instructionsTable[0x56] = Instruction{ &CPU::OP_56NN, 6, "LSR $nn,X", ZEROPAGEX };
	instructionsTable[0x58] = Instruction{ &CPU::OP_58, 2, "CLI", IMPLIED };
	instructionsTable[0x59] = Instruction{ &CPU::OP_59NN00, 4, "EOR $nnnn,Y", ABSOLUTEY };
	instructionsTable[0x5D] = Instruction{ &CPU::OP_5DNN00, 4, "EOR $nnnn,X", ABSOLUTEX };
	instructionsTable[0x5E] = Instruction{ &CPU::OP_5ENN00, 7, "LSR $nnnn,X", ABSOLUTEX };
	instructionsTable[0x60] = Instruction{ &CPU::OP_60, 6, "RTS", IMPLIED };
	instructionsTable[0x61] = Instruction{ &CPU::OP_61NN, 6, "ADC ($nn,X)", INDIRECTX };
	instructionsTable[0x65] = Instruction{ &CPU::OP_65NN, 3, "ADC $nn", ZEROPAGE };
	instructionsTable[0x66] = Instruction{ &CPU::OP_66NN, 5, "ROR $nn", ZEROPAGE };
	instructionsTable[0x68] = Instruction{ &CPU::OP_68, 4, "PLA", IMPLIED };
	instructionsTable[0x69] = Instruction{ &CPU::OP_69NN, 2, "ADC #$nn", IMMEDIATE };
	instructionsTable[0x6A] = Instruction{ &CPU::OP_6A, 2, "ROR A", ACCUMULATOR };
	instructionsTable[0x6C] = Instruction{ &CPU::OP_6CNN00, 5, "JMP ($nnnn)", ABSOLUTE };
	instructionsTable[0x6D] = Instruction{ &CPU::OP_6DNN00, 4, "ADC $nnnn", ABSOLUTE };
	instructionsTable[0x6E] = Instruction{ &CPU::OP_6ENN00, 6, "ROR $nnnn", ABSOLUTE };
	instructionsTable[0x70] = Instruction{ &CPU::OP_70NN, 2, "BVS $nnnn", RELATIVE };
	instructionsTable[0x71] = Instruction{ &CPU::OP_71NN, 5, "ADC ($nn),Y", INDIRECTY };
	instructionsTable[0x75] = Instruction{ &CPU::OP_75NN, 4, "ADC $nn,X", ZEROPAGEX };
	instructionsTable[0x76] = Instruction{ &CPU::OP_76NN, 6, "ROR $nn,X", ZEROPAGEX };
	instructionsTable[0x78] = Instruction{ &CPU::OP_78, 2, "SEI", IMPLIED };
	instructionsTable[0x79] = Instruction{ &CPU::OP_79NN00, 4, "ADC $nnnn,Y", ABSOLUTEY };
	instructionsTable[0x7D] = Instruction{ &CPU::OP_7DNN00, 4, "ADC $nnnn,X", ABSOLUTEX };
	instructionsTable[0x7E] = Instruction{ &CPU::OP_7ENN00, 7, "ROR $nnnn,X", ABSOLUTEX };
	instructionsTable[0x81] = Instruction{ &CPU::OP_81NN, 6, "STA ($nn,X)", INDIRECTX };
	instructionsTable[0x84] = Instruction{ &CPU::OP_84NN, 3, "STY $nn", ZEROPAGE };
	instructionsTable[0x85] = Instruction{ &CPU::OP_85NN, 3, "STA $nn", ZEROPAGE };
	instructionsTable[0x86] = Instruction{ &CPU::OP_86NN, 3, "STX $nn", ZEROPAGE };
	instructionsTable[0x88] = Instruction{ &CPU::OP_88, 2, "DEY", IMPLIED };
	instructionsTable[0x8A] = Instruction{ &CPU::OP_8A, 2, "TXA", IMPLIED };
	instructionsTable[0x8C] = Instruction{ &CPU::OP_8CNN00, 4, "STY $nnnn", ABSOLUTE };
	instructionsTable[0x8D] = Instruction{ &CPU::OP_8DNN00, 4, "STA $nnnn", ABSOLUTE };
	instructionsTable[0x8E] = Instruction{ &CPU::OP_8ENN00, 4, "STX $nnnn", ABSOLUTE };
	instructionsTable[0x90] = Instruction{ &CPU::OP_90NN, 2, "BCC $nnnn", RELATIVE };
	instructionsTable[0x91] = Instruction{ &CPU::OP_91NN, 6, "STA ($nn),Y", INDIRECTY };
	instructionsTable[0x94] = Instruction{ &CPU::OP_94NN, 4, "STY $nn,X", ZEROPAGEX };
	instructionsTable[0x95] = Instruction{ &CPU::OP_95NN, 4, "STA $nn,X", ZEROPAGEX };
	instructionsTable[0x96] = Instruction{ &CPU::OP_96NN, 4, "STX $nn,Y", ZEROPAGEY };
	instructionsTable[0x98] = Instruction{ &CPU::OP_98, 2, "TYA", IMPLIED };
	instructionsTable[0x99] = Instruction{ &CPU::OP_99NN00, 5, "STA $nnnn,Y", ABSOLUTEY };
	instructionsTable[0x9A] = Instruction{ &CPU::OP_9A, 2, "TXS", IMPLIED };
	instructionsTable[0x9D] = Instruction{ &CPU::OP_9DNN00, 5, "STA $nnnn,X", ABSOLUTEX };
	instructionsTable[0xA0] = Instruction{ &CPU::OP_A0NN, 2, "LDY #$nn", IMMEDIATE };
	instructionsTable[0xA1] = Instruction{ &CPU::OP_A1NN, 6, "LDA ($nn,X)", INDIRECTX };
	instructionsTable[0xA2] = Instruction{ &CPU::OP_A2NN, 2, "LDX #$nn", IMMEDIATE };
	instructionsTable[0xA4] = Instruction{ &CPU::OP_A4NN, 3, "LDY $nn", ZEROPAGE };
	instructionsTable[0xA5] = Instruction{ &CPU::OP_A5NN, 3, "LDA $nn", ZEROPAGE };
	instructionsTable[0xA6] = Instruction{ &CPU::OP_A6NN, 3, "LDX $nn", ZEROPAGE };
	instructionsTable[0xA8] = Instruction{ &CPU::OP_A8, 2, "TAY", IMPLIED };
	instructionsTable[0xA9] = Instruction{ &CPU::OP_A9NN, 2, "LDA #$nn", IMMEDIATE };
	instructionsTable[0xAA] = Instruction{ &CPU::OP_AA, 2, "TAX", IMPLIED };
	instructionsTable[0xAC] = Instruction{ &CPU::OP_ACNN00, 4, "LDY $nnnn", ABSOLUTE };
	instructionsTable[0xAD] = Instruction{ &CPU::OP_ADNN00, 4, "LDA $nnnn", ABSOLUTE };
	instructionsTable[0xAE] = Instruction{ &CPU::OP_AENN00, 4, "LDX $nnnn", ABSOLUTE };
	instructionsTable[0xB0] = Instruction{ &CPU::OP_B0NN, 2, "BCS $nnnn", RELATIVE };
	instructionsTable[0xB1] = Instruction{ &CPU::OP_B1NN, 5, "LDA ($nn),Y", INDIRECTY };
	instructionsTable[0xB4] = Instruction{ &CPU::OP_B4NN, 4, "LDY $nn,X", ZEROPAGEX };
	instructionsTable[0xB5] = Instruction{ &CPU::OP_B5NN, 4, "LDA $nn,X", ZEROPAGEX };
	instructionsTable[0xB6] = Instruction{ &CPU::OP_B6NN, 4, "LDX $nn,Y", ZEROPAGEY };
	instructionsTable[0xB8] = Instruction{ &CPU::OP_B8, 2, "CLV", IMPLIED };
	instructionsTable[0xB9] = Instruction{ &CPU::OP_B9NN00, 4, "LDA $nnnn,Y", ABSOLUTEY };
	instructionsTable[0xBA] = Instruction{ &CPU::OP_BA, 2, "TSX", IMPLIED };
	instructionsTable[0xBC] = Instruction{ &CPU::OP_BCNN00, 4, "LDY $nnnn,X", ABSOLUTEX };
	instructionsTable[0xBD] = Instruction{ &CPU::OP_BDNN00, 4, "LDA $nnnn,X", ABSOLUTEX };
	instructionsTable[0xBE] = Instruction{ &CPU::OP_BENN00, 4, "LDX $nnnn,Y", ABSOLUTEY };
	instructionsTable[0xC0] = Instruction{ &CPU::OP_C0NN, 2, "CPY #$nn", IMMEDIATE };
	instructionsTable[0xC1] = Instruction{ &CPU::OP_C1NN, 6, "CMP ($nn,X)", INDIRECTX };
	instructionsTable[0xC4] = Instruction{ &CPU::OP_C4NN, 3, "CPY $nn", ZEROPAGE };
	instructionsTable[0xC5] = Instruction{ &CPU::OP_C5NN, 3, "CMP $nn", ZEROPAGE };
	instructionsTable[0xC6] = Instruction{ &CPU::OP_C6NN, 5, "DEC $nn", ZEROPAGE };
	instructionsTable[0xC8] = Instruction{ &CPU::OP_C8, 2, "INY", IMPLIED };
	instructionsTable[0xC9] = Instruction{ &CPU::OP_C9NN, 2, "CMP #$nn", IMMEDIATE };
	instructionsTable[0xCA] = Instruction{ &CPU::OP_CA, 2, "DEX", IMPLIED };
	instructionsTable[0xCC] = Instruction{ &CPU::OP_CCNN00, 4, "CPY $nnnn", ABSOLUTE };
	instructionsTable[0xCD] = Instruction{ &CPU::OP_CDNN00, 4, "CMP $nnnn", ABSOLUTE };
	instructionsTable[0xCE] = Instruction{ &CPU::OP_CENN00, 6, "DEC $nnnn", ABSOLUTE };
	instructionsTable[0xD0] = Instruction{ &CPU::OP_D0NN, 2, "BNE $nnnn", RELATIVE };
	instructionsTable[0xD1] = Instruction{ &CPU::OP_D1NN, 5, "CMP ($nn),Y", INDIRECTY };
	instructionsTable[0xD5] = Instruction{ &CPU::OP_D5NN, 4, "CMP $nn,X", ZEROPAGEX };
	instructionsTable[0xD6] = Instruction{ &CPU::OP_D6NN, 6, "DEC $nn,X", ZEROPAGEX };
	instructionsTable[0xD8] = Instruction{ &CPU::OP_D8, 2, "CLD", IMPLIED };
	instructionsTable[0xD9] = Instruction{ &CPU::OP_D9NN00, 4, "CMP $nnnn,Y", ABSOLUTEY };
	instructionsTable[0xDD] = Instruction{ &CPU::OP_DDNN00, 4, "CMP $nnnn,X", ABSOLUTEX };
	instructionsTable[0xDE] = Instruction{ &CPU::OP_DENN00, 7, "DEC $nnnn,X", ABSOLUTEX };
	instructionsTable[0xE0] = Instruction{ &CPU::OP_E0NN, 2, "CPX #$nn", IMMEDIATE };
	instructionsTable[0xE1] = Instruction{ &CPU::OP_E1NN, 6, "SBC ($nn,X)", INDIRECTX };
	instructionsTable[0xE4] = Instruction{ &CPU::OP_E4NN, 3, "CPX $nn", ZEROPAGE };
	instructionsTable[0xE5] = Instruction{ &CPU::OP_E5NN, 3, "SBC $nn", ZEROPAGE };
	instructionsTable[0xE6] = Instruction{ &CPU::OP_E6NN, 5, "INC $nn", ZEROPAGE };
	instructionsTable[0xE8] = Instruction{ &CPU::OP_E8, 2, "INX", IMPLIED };
	instructionsTable[0xE9] = Instruction{ &CPU::OP_E9NN, 2, "SBC #$nn", IMMEDIATE };
	instructionsTable[0xEA] = Instruction{ &CPU::OP_EA, 2, "NOP", IMPLIED };
	instructionsTable[0xEC] = Instruction{ &CPU::OP_ECNN00, 4, "CPX $nnnn", ABSOLUTE };
	instructionsTable[0xED] = Instruction{ &CPU::OP_EDNN00, 4, "SBC $nnnn", ABSOLUTE };
	instructionsTable[0xEE] = Instruction{ &CPU::OP_EENN00, 6, "INC $nnnn", ABSOLUTE };
	instructionsTable[0xF0] = Instruction{ &CPU::OP_F0NN, 2, "BEQ $nnnn", RELATIVE };
	instructionsTable[0xF1] = Instruction{ &CPU::OP_F1NN, 5, "SBC ($nn),Y", INDIRECTY };
	instructionsTable[0xF5] = Instruction{ &CPU::OP_F5NN, 4, "SBC $nn,X", ZEROPAGEX };
	instructionsTable[0xF6] = Instruction{ &CPU::OP_F6NN, 6, "INC $nn,X", ZEROPAGEX };
	instructionsTable[0xF8] = Instruction{ &CPU::OP_F8, 2, "SED", IMPLIED };
	instructionsTable[0xF9] = Instruction{ &CPU::OP_F9NN00, 4, "SBC $nnnn,Y", ABSOLUTEY };
	instructionsTable[0xFD] = Instruction{ &CPU::OP_FDNN00, 4, "SBC $nnnn,X", ABSOLUTEX };
	instructionsTable[0xFE] = Instruction{ &CPU::OP_FENN00, 7, "INC $nnnn,X", ABSOLUTEX };
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
			memory.at(PRG_START_ADDRESS + i) = buffer[i + 16];
		}
	}
	else if (prgSize == 1)
	{
		for (int i = 0; i < 16384; i++)
		{
			memory.at(PRG_START_ADDRESS + i) = buffer[i + 16];
		}
		for (int i = 16384; i < 32768; i++)
		{
			memory.at(PRG_START_ADDRESS + i) = buffer[i + 16 - 16384];
		}
	}

	uint16_t mapperNumber = (buffer[7] & 0xF0) | ((buffer[6] & 0xF0) >> 4);
	setMapper(mapperNumber);

	delete[] buffer;

	uint8_t lowByteStartAddress = memory.at(RESET_VECTOR_ADDRESS);
	uint8_t highByteStartAddress = memory.at(RESET_VECTOR_ADDRESS + 1);

	uint16_t startAddress = (highByteStartAddress << 8) | lowByteStartAddress;
	pc = startAddress;
	currentInstruction = instructionsTable[memory[pc]];
}

void CPU::cycle() {
	int oldCycles = cycles;
	execute();
	ppu.cycles += (cycles - oldCycles) * 3;

	if (nmiInterrupt)
	{
		handleInterrupt('N');
		nmiInterrupt = false;
		cycles += 7;
	}
	if (irqInterrupt && !getFlag('I'))
	{
		handleInterrupt('I');
		irqInterrupt = false;
		cycles += 7;
	}
}

void CPU::step() {
	if (cyclesElapsed == currentInstruction.totalCycles)
	{
		cyclesElapsed = 0;
		execute();

		if (nmiInterrupt)
		{
			handleInterrupt('N');
			nmiInterrupt = false;
			cycles += 7;
		}
		if (irqInterrupt && !getFlag('I'))
		{
			handleInterrupt('I');
			irqInterrupt = false;
			cycles += 7;
		}
		currentInstruction = instructionsTable[memory[pc]];
	}
	else
	{
		cyclesElapsed++;
	}
}

void CPU::execute() {
	//printInfo();

	// Every instruction takes at least 2 cycles
	cycles += 2;
	//std::cout << "instruction executed: " << currentInstruction.instructionName << " | PC = 0x" << std::hex << pc << " | CPU-CYC = " << std::dec << cycles << std::endl;
	(this->*currentInstruction.function)();
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
		uint8_t lowByteVectorAddress = memory.at(IRQ_VECTOR_ADDRESS);
		uint8_t highByteVectorAddress = memory.at(IRQ_VECTOR_ADDRESS + 1);
		uint16_t vectorAddress = (highByteVectorAddress << 8) | lowByteVectorAddress;
		pc = vectorAddress;

		break;
	}
	// NMI interrupt
	case 'N':
	{
		uint8_t lowByteVectorAddress = memory.at(NMI_VECTOR_ADDRESS);
		uint8_t highByteVectorAddress = memory.at(NMI_VECTOR_ADDRESS + 1);
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
	std::cout << " PPUSC:" << std::dec << ppu.scanlines;
	std::cout << " PPUCYC:" << ppu.cycles;
	std::cout << " CYC:" << std::dec << cycles;
	std::cout << std::endl;
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

void CPU::setMapper(uint16_t mapperNumber) {
	switch (mapperNumber)
	{
	case 0:
		this->mapper = std::make_shared<Mapper>(this, &this->ppu);
		break;
	case 3:
		this->mapper = std::make_shared<Mapper003>(this, &this->ppu);
		break;
	}
	this->ppu.mapper = this->mapper;
}

void CPU::stepPpu() {
	unsigned int currentPpuCycleCount = this->ppu.cycles;
	unsigned int steps = currentPpuCycleCount - previousPpuCycleCount;
	//std::cout << "steps: " << steps << std::endl;
	for (int i = 0; i < steps; i++)
	{
		this->ppu.step(this);
	}
	previousPpuCycleCount = currentPpuCycleCount;
}

void CPU::stepPpu(int steps) {
	for (int i = 0; i < steps; i++)
	{
		this->ppu.step(this);
	}
}

void CPU::incrementCycle(int _cycles) {
	this->cycles += _cycles;
	//stepPpu(_cycles * 3);
}