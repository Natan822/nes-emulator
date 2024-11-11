#include <stdint.h>
#include <iostream>
#include <sstream>
#include "CPU.h"

void CPU::invalid() {
	std::ostringstream errorMessage;
	errorMessage << 
		"Invalid or unimplemented OPCODE: 0x" << std::hex << static_cast<int>(this->memory[pc]) << " at PC : 0x"  << this->pc;

	std::cerr << errorMessage.str() << std::endl;
	throw std::runtime_error(errorMessage.str());
}

void CPU::ADC(uint8_t value) {

	uint16_t result = value + aReg;
	if (getFlag('C'))
	{
		result++;
	}

	setFlag('C', result > 0xFF);
	setFlag('Z', (result & 0xFF) == 0);
	setFlag('N', result & 0x80);

	// Set V flag to 1 if signals of value and aReg are the same but signal of result is different, which means the signal of result is wrong
	setFlag('V', (~(aReg ^ value) & (result ^ aReg) & 0x80));

	aReg = result & 0xFF;
}

void CPU::OP_69NN() {
	uint8_t value = immediate();

	ADC(value);

	pc += 2;
}

void CPU::OP_65NN() {
	uint8_t value = zeroPage();

	ADC(value);

	pc += 2;
}

void CPU::OP_75NN() {
	uint8_t value = zeroPageX();

	ADC(value);

	pc += 2;
}

void CPU::OP_6DNN00() {
	uint8_t value = absolute();

	ADC(value);

	pc += 3;
}
void CPU::OP_7DNN00() {
	uint8_t value = absoluteX();

	ADC(value);

	pc += 3;
}

void CPU::OP_79NN00() {
	uint8_t value = absoluteY();

	ADC(value);

	pc += 3;
}

void CPU::OP_61NN() {
	uint8_t value = indirectX();

	ADC(value);

	pc += 2;
}

void CPU::OP_71NN() {
	uint8_t value = indirectY();

	ADC(value);

	pc += 2;
}

void CPU::AND(uint8_t value) {
	aReg &= value;

	// Set the zero flag
	if (aReg == 0)
	{
		status |= 0x02;
	}
	else
	{
		status &= ~0x02;
	}

	// Set the negative flag
	if (aReg & 0x80)
	{
		status |= 0x80;
	}
	else
	{
		status &= ~0x80;
	}
}
void CPU::OP_29NN() {
	uint8_t value = immediate();
	AND(value);

	pc += 2;
}

void CPU::OP_25NN() {
	uint8_t value = zeroPage();
	AND(value);

	pc += 2;
}

void CPU::OP_35NN() {
	uint8_t value = zeroPageX();
	AND(value);

	pc += 2;
}

void CPU::OP_2DNN00() {
	uint8_t value = absolute();
	AND(value);

	pc += 3;
}

void CPU::OP_3DNN00() {
	uint8_t value = absoluteX();
	AND(value);

	pc += 3;
}

void CPU::OP_39NN00() {
	uint8_t value = absoluteY();
	AND(value);

	pc += 3;
}
void CPU::OP_21NN() {
	uint8_t value = indirectX();
	AND(value);

	pc += 2;
}
void CPU::OP_31NN() {
	uint8_t value = indirectY();
	AND(value);

	pc += 2;
}

void CPU::ASL(uint16_t address) {
	uint8_t value = readMemory(address);

	// Most Significant Bit
	uint8_t msb = (value & 0x80) >> 7;

	value = writeMemory(address, value << 1);
	// Carry flag = MSB
	setFlag('C', msb);
	setFlag('Z', value == 0);
	setFlag('N', value & 0x80);

	cycles += 2;
}

void CPU::OP_0A() {

	// Most Significant Bit
	uint8_t msb = (aReg & 0x80) >> 7;

	aReg <<= 1;

	// Carry flag = MSB
	setFlag('C', msb);
	setFlag('Z', aReg == 0);
	setFlag('N', aReg & 0x80);

	pc += 1;
}

void CPU::OP_06NN() {
	uint16_t address = zeroPagePtr();
	ASL(address);

	pc += 2;
}
void CPU::OP_16NN() {
	uint16_t address = zeroPageXPtr();
	ASL(address);

	pc += 2;
}
void CPU::OP_0ENN00() {
	uint16_t address = absolutePtr();
	ASL(address);

	pc += 3;
}

void CPU::OP_1ENN00() {
	uint16_t address = absoluteXPtr();
	ASL(address);

	// Make sure ASL(absolute, X) takes 7 cycles even if page crossing doesn't occur
	if ((memory[pc + 1] + xReg) <= 0xFF)
	{
		cycles++;
	}
	pc += 3;
}

void CPU::BIT(uint8_t value) {
	uint8_t result = value & aReg;

	// Set the zero flag
	if (result == 0)
	{
		status |= 0x02;
	}
	else
	{
		status &= ~0x02;
	}

	// Set Negative flag to match bit 7 of value
	if (value & 0x80)
	{
		status |= 0x80;
	}
	else
	{
		status &= ~0x80;
	}

	// Set Overflow flag to match bit 6 of value;
	if (value & 0x40)
	{
		status |= 0x40;
	}
	else
	{
		status &= ~0x40;
	}
}

void CPU::OP_24NN() {
	uint8_t value = zeroPage();
	BIT(value);

	pc += 2;
}

void CPU::OP_2CNN00() {
	uint8_t value = absolute();
	BIT(value);

	pc += 3;
}

void CPU::BRANCH() {
	cycles++;

	int8_t signedBranchValue = static_cast<int8_t>(memory[pc - 1]);

	// Page boundary crossed
	if (((pc & 0xFF) + signedBranchValue) > 0xFF)
	{
		cycles++;
	}

	pc += signedBranchValue;
}

void CPU::OP_90NN() {
	pc += 2;

	// Carry flag = 0
	if (!getFlag('C'))
	{
		BRANCH();
	}
}

void CPU::OP_B0NN() {
	pc += 2;

	// Carry flag = 1
	if (status & 0x1)
	{
		BRANCH();
	}
}

void CPU::OP_F0NN() {
	pc += 2;

	// Zero flag = 1
	if (status & 0x2)
	{
		BRANCH();
	}
}

void CPU::OP_30NN() {
	pc += 2;

	// Negative flag = 1
	if (status & 0x80)
	{
		BRANCH();
	}
}

void CPU::OP_D0NN() {
	pc += 2;

	// Zero flag = 0
	if (!getFlag('Z'))
	{
		BRANCH();
	}
}

void CPU::OP_10NN() {
	pc += 2;

	// Negative flag = 0
	if (!getFlag('N'))
	{
		BRANCH();
	}
}

void CPU::OP_50NN() {
	pc += 2;

	// Overflow flag = 0
	if (!getFlag('V'))
	{
		BRANCH();
	}
}

void CPU::OP_70NN() {
	pc += 2;

	// Overflow flag = 1
	if (status & 0x40)
	{
		BRANCH();
	}
}
void CPU::OP_00NN() {
	setFlag('B', 1);

	uint16_t address = pc + 2;

	push(address >> 8);
	push(address & 0xFF);

	setFlag('B', 0);
	push(status);
	setFlag('I', 1);

	uint8_t lowByte = memory[0xFFFE];
	uint8_t highByte = memory[0xFFFF];

	pc = (highByte << 8) | (lowByte);
	cycles += 5;
}

void CPU::CMP(uint8_t value) {
	uint16_t result = aReg - value;

	setFlag('C', aReg >= value);

	setFlag('Z', (result & 0xFF) == 0);

	setFlag('N', result & 0x80);
}

void CPU::OP_C9NN() {
	uint8_t value = immediate();
	CMP(value);

	pc += 2;
}

void CPU::OP_C5NN() {
	uint8_t value = zeroPage();
	CMP(value);

	pc += 2;
}

void CPU::OP_D5NN() {
	uint8_t value = zeroPageX();
	CMP(value);

	pc += 2;
}

void CPU::OP_CDNN00() {
	uint8_t value = absolute();
	CMP(value);

	pc += 3;
}

void CPU::OP_DDNN00() {
	uint8_t value = absoluteX();
	CMP(value);

	pc += 3;
}

void CPU::OP_D9NN00() {
	uint8_t value = absoluteY();
	CMP(value);

	pc += 3;
}

void CPU::OP_C1NN() {
	uint8_t value = indirectX();
	CMP(value);

	pc += 2;
}
void CPU::OP_D1NN() {
	uint8_t value = indirectY();
	CMP(value);

	pc += 2;
}

void CPU::CPX(uint8_t value) {
	uint16_t result = xReg - value;

	setFlag('C', xReg >= value);

	setFlag('Z', (result & 0xFF) == 0);

	setFlag('N', result & 0x80);
}

void CPU::OP_E0NN() {
	uint8_t value = immediate();
	CPX(value);

	pc += 2;
}

void CPU::OP_E4NN() {
	uint8_t value = zeroPage();
	CPX(value);

	pc += 2;
}

void CPU::OP_ECNN00() {
	uint8_t value = absolute();
	CPX(value);

	pc += 3;
}

void CPU::CPY(uint8_t value) {
	uint16_t result = yReg - value;

	setFlag('C', yReg >= value);

	setFlag('Z', (result & 0xFF) == 0);

	setFlag('N', result & 0x80);
}

void CPU::OP_C0NN() {
	uint8_t value = immediate();
	CPY(value);

	pc += 2;
}

void CPU::OP_C4NN() {
	uint8_t value = zeroPage();
	CPY(value);

	pc += 2;
}

void CPU::OP_CCNN00() {
	uint8_t value = absolute();
	CPY(value);

	pc += 3;
}

void CPU::DEC(uint16_t address) {
	uint8_t value = readMemory(address);
	value = writeMemory(address, value - 1);
	
	setFlag('N', value & 0x80);
	setFlag('Z', value == 0);

	cycles += 2;
}

void CPU::OP_C6NN() {
	uint16_t address = zeroPagePtr();
	DEC(address);

	pc += 2;
}

void CPU::OP_D6NN() {
	uint16_t address = zeroPageXPtr();
	DEC(address);

	pc += 2;
}

void CPU::OP_CENN00() {
	uint16_t address = absolutePtr();
	DEC(address);

	pc += 3;
}

void CPU::OP_DENN00() {
	uint16_t address = absoluteXPtr();
	DEC(address);

	// Make sure DEC(absolute, X) always takes 7 cycles even if page crossing doesn't occur
	if ((memory[pc + 1] + xReg) <= 0xFF)
	{
		cycles++;
	}
	pc += 3;
}

void CPU::EOR(uint8_t value) {
	aReg ^= value;

	setFlag('N', (aReg & 0x80));
	setFlag('Z', (aReg == 0));
}

void CPU::OP_49NN() {
	uint8_t value = immediate();
	EOR(value);

	pc += 2;
}

void CPU::OP_45NN() {
	uint8_t value = zeroPage();
	EOR(value);

	pc += 2;
}

void CPU::OP_55NN() {
	uint8_t value = zeroPageX();
	EOR(value);

	pc += 2;
}

void CPU::OP_4DNN00() {
	uint8_t value = absolute();
	EOR(value);

	pc += 3;
}

void CPU::OP_5DNN00() {
	uint8_t value = absoluteX();
	EOR(value);

	pc += 3;
}

void CPU::OP_59NN00() {
	uint8_t value = absoluteY();
	EOR(value);

	pc += 3;
}

void CPU::OP_41NN() {
	uint8_t value = indirectX();
	EOR(value);

	pc += 2;
}

void CPU::OP_51NN() {
	uint8_t value = indirectY();
	EOR(value);

	pc += 2;
}

void CPU::OP_18() {
	setFlag('C', 0);
	pc += 1;
}
void CPU::OP_38() {
	setFlag('C', 1);
	pc += 1;
}

void CPU::OP_58() {
	setFlag('I', 0);
	pc += 1;
}

void CPU::OP_78() {
	setFlag('I', 1);
	pc += 1;
}

void CPU::OP_B8() {
	setFlag('V', 0);
	pc += 1;
}

void CPU::OP_D8() {
	setFlag('D', 0);
	pc += 1;
}

void CPU::OP_F8() {
	setFlag('D', 1);
	pc += 1;
}

void CPU::INC(uint16_t address) {
	uint8_t value = readMemory(address);
	value = writeMemory(address, value + 1);

	setFlag('N', value & 0x80);
	setFlag('Z', value == 0);

	cycles += 2;
}

void CPU::OP_E6NN() {
	uint16_t address = zeroPagePtr();
	INC(address);

	pc += 2;
}

void CPU::OP_F6NN() {
	uint16_t address = zeroPageXPtr();
	INC(address);

	pc += 2;
}

void CPU::OP_EENN00() {
	uint16_t address = absolutePtr();
	INC(address);

	pc += 3;
}

void CPU::OP_FENN00() {
	uint16_t address = absoluteXPtr();
	INC(address);

	// Make sure INC(absolute, X) always takes 7 cycles even if page crossing doesn't occur
	if ((memory[pc + 1] + xReg) <= 0xFF)
	{
		cycles++;
	}
	pc += 3;
}

void CPU::JMP(uint16_t address) {
	pc = address;
}

void CPU::OP_4CNN00() {
	uint8_t lowByte = memory[pc + 1];
	uint8_t highByte = memory[pc + 2];

	uint16_t address = (highByte << 8) | lowByte;
	JMP(address);

	cycles++;
}

void CPU::OP_6CNN00() {
	uint8_t lowBytePtr = memory[pc + 1];
	uint8_t highBytePtr = memory[pc + 2];

	uint16_t addressPtr = (highBytePtr << 8) | lowBytePtr;

	uint16_t address{};
	uint8_t lowByte = memory[addressPtr];
	if (lowBytePtr == 0xFF)
	{
		uint8_t highByte = memory[addressPtr - 0xFF];

		address = (highByte << 8) | lowByte;
	}
	else
	{
		uint8_t highByte = memory[addressPtr + 1];

		address = (highByte << 8) | lowByte;
	}
	JMP(address);

	cycles += 3;
}

void CPU::OP_20NN00() {
	uint8_t lowByte = memory[pc + 1];
	uint8_t highByte = memory[pc + 2];

	uint16_t address = (highByte << 8) | lowByte;

	// Push high-byte of return address - 1
	push((pc + 2) >> 8);
	// Push low-byte of return address - 1
	push((pc + 2) & 0x00FF);

	JMP(address);
	cycles += 4;
}

void CPU::LDA(uint8_t value) {
	aReg = value;

	setFlag('N', aReg & 0x80);
	setFlag('Z', aReg == 0);
}

void CPU::OP_A9NN() {
	uint8_t value = immediate();

	LDA(value);
	pc += 2;
}

void CPU::OP_A5NN() {
	uint8_t value = zeroPage();

	LDA(value);
	pc += 2;
}

void CPU::OP_B5NN() {
	uint8_t value = zeroPageX();

	LDA(value);
	pc += 2;
}

void CPU::OP_ADNN00() {
	uint8_t value = absolute();

	LDA(value);
	pc += 3;
}

void CPU::OP_BDNN00() {
	uint8_t value = absoluteX();

	LDA(value);
	pc += 3;
}

void CPU::OP_B9NN00() {
	uint8_t value = absoluteY();

	LDA(value);
	pc += 3;
}

void CPU::OP_A1NN() {
	uint8_t value = indirectX();
	 
	LDA(value);
	pc += 2;
}

void CPU::OP_B1NN() {
	uint8_t value = indirectY();

	LDA(value);
	pc += 2;
}

void CPU::LDX(uint8_t value) {
	xReg = value;

	setFlag('N', xReg & 0x80);
	setFlag('Z', xReg == 0);
}

void CPU::OP_A2NN() {
	uint8_t value = immediate();

	LDX(value);
	pc += 2;
}

void CPU::OP_A6NN() {
	uint8_t value = zeroPage();

	LDX(value);
	pc += 2;
}

void CPU::OP_B6NN() {
	uint8_t value = zeroPageY();

	LDX(value);
	pc += 2;
}

void CPU::OP_AENN00() {
	uint8_t value = absolute();

	LDX(value);
	pc += 3;
}

void CPU::OP_BENN00() {
	uint8_t value = absoluteY();

	LDX(value);
	pc += 3;
}

void CPU::LDY(uint8_t value) {
	yReg = value;

	setFlag('N', yReg & 0x80);
	setFlag('Z', yReg == 0);
}

void CPU::OP_A0NN() {
	uint8_t value = immediate();

	LDY(value);
	pc += 2;
}

void CPU::OP_A4NN() {
	uint8_t value = zeroPage();

	LDY(value);
	pc += 2;
}

void CPU::OP_B4NN() {
	uint8_t value = zeroPageX();

	LDY(value);
	pc += 2;
}

void CPU::OP_ACNN00() {
	uint8_t value = absolute();

	LDY(value);
	pc += 3;
}

void CPU::OP_BCNN00() {
	uint8_t value = absoluteX();

	LDY(value);
	pc += 3;
}

void CPU::LSR(uint16_t address) {
	uint8_t value = readMemory(address);
	uint8_t bitZero = value & 0x01;

	value = writeMemory(address, value >> 1);

	setFlag('C', bitZero);
	setFlag('N', value & 0x80);
	setFlag('Z', value == 0);

	cycles += 2;
}

void CPU::OP_4A() {
	uint8_t bitZero = aReg & 0x01;

	aReg >>= 1;

	setFlag('C', bitZero);
	setFlag('N', aReg & 0x80);
	setFlag('Z', aReg == 0);

	pc += 1;
}

void CPU::OP_46NN() {
	uint16_t address = zeroPagePtr();
	LSR(address);

	pc += 2;
}

void CPU::OP_56NN() {
	uint16_t address = zeroPageXPtr();
	LSR(address);

	pc += 2;
}

void CPU::OP_4ENN00() {
	uint16_t address = absolutePtr();
	LSR(address);

	pc += 3;
}

void CPU::OP_5ENN00() {
	uint16_t address = absoluteXPtr();
	LSR(address);

	// Make sure LSR(absolute, X) always takes 7 cycles even if page crossing doesn't occur
	if ((memory[pc + 1] + xReg) <= 0xFF)
	{
		cycles++;
	}
	pc += 3;
}

void CPU::OP_EA() {
	pc += 1;
}

void CPU::ORA(uint8_t value) {
	aReg |= value;
	
	setFlag('N', aReg & 0x80);
	setFlag('Z', aReg == 0);
}

void CPU::OP_09NN() {
	uint8_t value = immediate();

	ORA(value);
	pc += 2;
}

void CPU::OP_05NN() {
	uint8_t value = zeroPage();

	ORA(value);
	pc += 2;
}

void CPU::OP_15NN() {
	uint8_t value = zeroPageX();

	ORA(value);
	pc += 2;
}

void CPU::OP_0DNN00() {
	uint8_t value = absolute();

	ORA(value);
	pc += 3;
}

void CPU::OP_1DNN00() {
	uint8_t value = absoluteX();

	ORA(value);
	pc += 3;
}

void CPU::OP_19NN00() {
	uint8_t value = absoluteY();

	ORA(value);
	pc += 3;
}

void CPU::OP_01NN() {
	uint8_t value = indirectX();

	ORA(value);
	pc += 2;
}

void CPU::OP_11NN() {
	uint8_t value = indirectY();

	ORA(value);
	pc += 2;
}

void CPU::OP_AA() {
	xReg = aReg;

	setFlag('N', xReg & 0x80);
	setFlag('Z', xReg == 0);

	pc += 1;
}

void CPU::OP_8A() {
	aReg = xReg;

	setFlag('N', aReg & 0x80);
	setFlag('Z', aReg == 0);

	pc += 1;
}

void CPU::OP_CA() {
	xReg--;

	setFlag('N', xReg & 0x80);
	setFlag('Z', xReg == 0);

	pc += 1;
}

void CPU::OP_E8() {
	xReg++;

	setFlag('N', xReg & 0x80);
	setFlag('Z', xReg == 0);

	pc += 1;
}

void CPU::OP_A8() {
	yReg = aReg;

	setFlag('N', yReg & 0x80);
	setFlag('Z', yReg == 0);

	pc += 1;
}

void CPU::OP_98() {
	aReg = yReg;

	setFlag('N', aReg & 0x80);
	setFlag('Z', aReg == 0);

	pc += 1;
}

void CPU::OP_88() {
	yReg--;

	setFlag('N', yReg & 0x80);
	setFlag('Z', yReg == 0);

	pc += 1;
}

void CPU::OP_C8() {
	yReg++;

	setFlag('N', yReg & 0x80);
	setFlag('Z', yReg == 0);

	pc += 1;
}

void CPU::ROL(uint16_t address) {
	uint8_t carryFlag = getFlag('C');

	uint8_t value = readMemory(address);
	setFlag('C', value & 0x80);

	value = writeMemory(address, (value << 1) | carryFlag);

	setFlag('N', value & 0x80);
	setFlag('Z', value == 0);

	cycles += 2;
}

void CPU::OP_2A() {
	uint8_t carryFlag = getFlag('C');

	setFlag('C', aReg & 0x80);

	aReg = (aReg << 1) | carryFlag;

	setFlag('N', aReg & 0x80);
	setFlag('Z', aReg == 0);

	pc += 1;
}

void CPU::OP_26NN() {
	uint16_t address = zeroPagePtr();
	ROL(address);

	pc += 2;
}

void CPU::OP_36NN() {
	uint16_t address = zeroPageXPtr();
	ROL(address);

	pc += 2;
}

void CPU::OP_2ENN00() {
	uint16_t address = absolutePtr();
	ROL(address);

	pc += 3;
}

void CPU::OP_3ENN00() {
	uint16_t address = absoluteXPtr();
	ROL(address);

	// Make sure ROL(absolute, X) always takes 7 cycles even if page crossing doesn't occur
	if ((memory[pc + 1] + xReg) <= 0xFF)
	{
		cycles++;
	}
	pc += 3;
}

void CPU::ROR(uint16_t address) {
	uint8_t value = readMemory(address);
	uint8_t bitZero = value & 0x1;
	uint8_t carryFlag = getFlag('C');

	value = writeMemory(address, (value >> 1) | (carryFlag << 7));

	setFlag('C', bitZero);
	setFlag('N', value & 0x80);
	setFlag('Z', value == 0);

	cycles += 2;
}

void CPU::OP_6A() {
	uint8_t bitZero = aReg & 0x1;
	uint8_t carryFlag = getFlag('C');

	aReg = (aReg >> 1) | (carryFlag << 7);

	setFlag('C', bitZero);
	setFlag('N', aReg & 0x80);
	setFlag('Z', aReg == 0);
	pc += 1;
}

void CPU::OP_66NN() {
	uint16_t address = zeroPagePtr();
	ROR(address);

	pc += 2;
}

void CPU::OP_76NN() {
	uint16_t address = zeroPageXPtr();
	ROR(address);

	pc += 2;
}

void CPU::OP_6ENN00() {
	uint16_t address = absolutePtr();
	ROR(address);

	pc += 3;
}

void CPU::OP_7ENN00() {
	uint16_t address = absoluteXPtr();
	ROR(address);

	// Make sure ROR(absolute, X) takes 7 cycles even if page crossing doesn't occur
	if ((memory[pc + 1] + xReg) <= 0xFF)
	{
		cycles++;
	}
	pc += 3;
}

void CPU::OP_40() {
	status = pop();
	setFlag('B', 0);
	status |= 0x24;

	uint8_t pcLowByte = pop();
	uint8_t pcHighByte = pop();

	uint16_t pcAddress = (pcHighByte << 8) | pcLowByte;
	pc = pcAddress;
	cycles += 4;
}

void CPU::OP_60() {
	uint8_t pcLowByte = pop();
	uint8_t pcHighByte = pop();

	uint16_t pcAddress = (pcHighByte << 8) | pcLowByte;
	pc = pcAddress + 1;
	cycles += 4;
}

void CPU::SBC(uint8_t value) {
	uint8_t reverseCarry = !getFlag('C');
	uint16_t result = aReg - value - reverseCarry;

	setFlag('C', result <= 0xFF);
	setFlag('Z', (result & 0xFF) == 0);
	setFlag('N', result & 0x80);
	
	// If signals of aReg and value are different, the signal of result must be the same as aReg. Otherwise V flag is set to 1
	setFlag('V', ((aReg ^ value) & (aReg ^ result) & 0x80));

	aReg = result & 0xFF;
}

void CPU::OP_E9NN() {
	uint8_t value = immediate();
	SBC(value);

	pc += 2;
}

void CPU::OP_E5NN() {
	uint8_t value = zeroPage();
	SBC(value);

	pc += 2;
}

void CPU::OP_F5NN() {
	uint8_t value = zeroPageX();
	SBC(value);

	pc += 2;
}

void CPU::OP_EDNN00() {
	uint8_t value = absolute();
	SBC(value);

	pc += 3;
}

void CPU::OP_FDNN00() {
	uint8_t value = absoluteX();
	SBC(value);

	pc += 3;
}

void CPU::OP_F9NN00() {
	uint8_t value = absoluteY();
	SBC(value);

	pc += 3;
}

void CPU::OP_E1NN() {
	uint8_t value = indirectX();
	SBC(value);

	pc += 2;
}

void CPU::OP_F1NN() {
	uint8_t value = indirectY();
	SBC(value);

	pc += 2;
}

void CPU::STA(uint16_t address) {
	writeMemory(address, aReg);
}

void CPU::OP_85NN() {
	uint16_t address = zeroPagePtr();
	STA(address);

	pc += 2;
}

void CPU::OP_95NN() {
	uint16_t address = zeroPageXPtr();
	STA(address);

	pc += 2;
}

void CPU::OP_8DNN00() {
	uint16_t address = absolutePtr();
	STA(address);

	pc += 3;
}

void CPU::OP_9DNN00() {
	uint16_t address = absoluteXPtr();
	STA(address);

	// Make sure STA(absolute, X) takes 5 cycles even if page crossing doesn't occur
	if ((memory[pc + 1] + xReg) <= 0xFF)
	{
		cycles++;
	}
	pc += 3;
}

void CPU::OP_99NN00() {
	uint16_t address = absoluteYPtr();
	STA(address);

	cycles++;
	pc += 3;
}

void CPU::OP_81NN() {
	uint16_t address = indirectXPtr();
	STA(address);

	pc += 2;
}

void CPU::OP_91NN() {
	uint16_t address = indirectYPtr();
	STA(address);

	cycles++;
	pc += 2;
}

void CPU::OP_9A() {
	sp = xReg;
	pc += 1;
}

void CPU::OP_BA() {
	xReg = sp;

	setFlag('N', xReg & 0x80);
	setFlag('Z', xReg == 0);

	pc += 1;
}

void CPU::OP_48() {
	push(aReg);

	pc += 1;
	cycles++;
}

void CPU::OP_68() {
	aReg = pop();

	setFlag('N', aReg & 0x80);
	setFlag('Z', aReg == 0);

	pc += 1;
	cycles += 2;
}

void CPU::OP_08() {
	push(status);

	pc += 1;
	cycles++;
}

void CPU::OP_28() {
	status = pop();

	status |= 0x20;
	setFlag('B', 0);

	pc += 1;
	cycles += 2;
}

void CPU::STX(uint16_t address) {
	writeMemory(address, xReg);
}

void CPU::OP_86NN() {
	uint16_t address = zeroPagePtr();
	STX(address);

	pc += 2;
}

void CPU::OP_96NN() {
	uint16_t address = zeroPageYPtr();
	STX(address);

	pc += 2;
}

void CPU::OP_8ENN00() {
	uint16_t address = absolutePtr();
	STX(address);

	pc += 3;
}

void CPU::STY(uint16_t address) {
	writeMemory(address, yReg);
}

void CPU::OP_84NN() {
	uint16_t address = zeroPagePtr();
	STY(address);

	pc += 2;
}

void CPU::OP_94NN() {
	uint16_t address = zeroPageXPtr();
	STY(address);

	pc += 2;
}

void CPU::OP_8CNN00() {
	uint16_t address = absolutePtr();
	STY(address);

	pc += 3;
}