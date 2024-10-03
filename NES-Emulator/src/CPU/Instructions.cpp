#include <stdint.h>
#include "CPU.h"

void CPU::ADC(uint8_t value) {

	int16_t result = value + aReg;

	setFlag('C', result > 0xFF);
	setFlag('Z', result == 0);
	setFlag('N', result < 0);

	// Set V flag to 1 if signals of value and aReg are the same but signal of result is different, which means the signal of result is wrong
	setFlag('V', (~(xReg ^ value) & (result ^ xReg) & 0x80));

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

void CPU::ASL(uint8_t* value) {
	// Most Significant Byte
	uint8_t msb = (*value & 0x80) >> 7;
	// Clear carry flag
	status &= ~0x1;
	// Set carry flag = MSB
	status |= msb;

	*value <<= 1;

	// Set the zero flag
	if (*value == 0)
	{
		status |= 0x02;
	}
	else
	{
		status &= ~0x02;
	}

	// Set the negative flag
	if (*value & 0x80)
	{
		status |= 0x80;
	}
	else
	{
		status &= ~0x80;
	}

}

void CPU::OP_0A() {
	uint8_t* value = &aReg;
	ASL(value);

	pc += 1;
}

void CPU::OP_06NN() {
	uint8_t address = memory[pc + 1];
	uint8_t* value = &(memory[address]);
	ASL(value);

	pc += 2;
}
void CPU::OP_16NN() {
	uint8_t address = memory[pc + 1];
	uint8_t* value = &(memory[address + xReg]);
	ASL(value);

	pc += 2;
}
void CPU::OP_0ENN00() {
	uint8_t lowByte = memory[pc + 1];
	uint8_t highByte = memory[pc + 2];

	uint16_t address = highByte << 8 + lowByte;
	uint8_t* value = &(memory[address]);
	ASL(value);

	pc += 3;
}

void CPU::OP_1ENN00() {
	uint8_t lowByte = memory[pc + 1];
	uint8_t highByte = memory[pc + 2];

	uint16_t address = highByte << 8 + lowByte;
	uint8_t* value = &(memory[address + xReg]);
	ASL(value);

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

void CPU::BRANCH(uint8_t value) {
	int8_t signedValue = static_cast<int8_t>(value);
	pc += signedValue;
}

void CPU::OP_90NN() {
	// Carry flag = 1
	if (status & 0x1)
	{
		pc += 2;
	}
	// Carry flag = 0
	else
	{
		BRANCH(memory[pc + 1]);
	}
}

void CPU::OP_B0NN() {
	// Carry flag = 1
	if (status & 0x1)
	{
		BRANCH(memory[pc + 1]);
	}
	// Carry flag = 0
	else
	{
		pc += 2;
	}
}

void CPU::OP_F0NN() {
	// Zero flag = 1
	if (status & 0x2)
	{
		BRANCH(memory[pc + 1]);
	}
	// Zero flag = 0
	else
	{
		pc += 2;
	}
}

void CPU::OP_30NN() {
	// Negative flag = 1
	if (status & 0x80)
	{
		BRANCH(memory[pc + 1]);
	}
	// Negative flag = 0
	else
	{
		pc += 2;
	}
}

void CPU::OP_D0NN() {
	// Zero flag = 1
	if (status & 0x2)
	{
		pc += 2;
	}
	// Zero flag = 0
	else
	{
		BRANCH(memory[pc + 1]);
	}
}

void CPU::OP_50NN() {
	// Overflow flag = 1
	if (status & 0x40)
	{
		pc += 2;
	}
	// Overflow flag = 0
	else
	{
		BRANCH(memory[pc + 1]);
	}
}

void CPU::OP_70NN() {
	// Overflow flag = 1
	if (status & 0x40)
	{
		BRANCH(memory[pc + 1]);
	}
	// Overflow flag = 0
	else
	{
		pc += 2;
	}
}
void CPU::OP_00NN() {
	setFlag('B', 1);

	push(pc + 2);
	push(status);
}

void CPU::CMP(uint8_t value) {
	int16_t result = aReg - value;

	setFlag('C', aReg >= value);

	setFlag('Z', result == 0);

	setFlag('N', result < 0);
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
	int16_t result = xReg - value;

	setFlag('C', xReg >= value);

	setFlag('Z', result == 0);

	setFlag('N', result < 0);
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
	int16_t result = yReg - value;

	setFlag('C', yReg >= value);

	setFlag('Z', result == 0);

	setFlag('N', result < 0);
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

void CPU::DEC(uint8_t* value) {
	(*value)--;

	setFlag('N', (*value & 0x80));
	setFlag('Z', (*value == 0));
}

void CPU::OP_C6NN() {
	uint8_t* value = zeroPagePtr();
	DEC(value);

	pc += 2;
}

void CPU::OP_D6NN() {
	uint8_t* value = zeroPageXPtr();
	DEC(value);

	pc += 2;
}

void CPU::OP_CENN00() {
	uint8_t* value = absolutePtr();
	DEC(value);

	pc += 3;
}

void CPU::OP_DENN00() {
	uint8_t* value = absoluteXPtr();
	DEC(value);

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

void CPU::INC(uint8_t* value) {
	(*value)++;

	setFlag('N', (*value & 0x80));
	setFlag('Z', (*value == 0));
}

void CPU::OP_E6NN() {
	uint8_t* value = zeroPagePtr();
	INC(value);

	pc += 2;
}

void CPU::OP_F6NN() {
	uint8_t* value = zeroPageXPtr();
	INC(value);

	pc += 2;
}

void CPU::OP_EENN00() {
	uint8_t* value = absolutePtr();
	INC(value);

	pc += 3;
}

void CPU::OP_FENN00() {
	uint8_t* value = absoluteXPtr();
	INC(value);

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
}

void CPU::OP_20NN00() {
	uint8_t lowByte = memory[pc + 1];
	uint8_t highByte = memory[pc + 2];

	uint16_t address = (highByte << 8) | lowByte;

	// Push high-byte of return address
	push((pc + 3) >> 8);
	// Push low-byte of return address
	push((pc + 3) & 0x00FF);

	JMP(address);
}

void CPU::LDA(uint8_t value) {
	aReg = value;

	setFlag('N', value & 0x80);
	setFlag('Z', value == 0);
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

	setFlag('N', value & 0x80);
	setFlag('Z', value == 0);
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

	setFlag('N', value & 0x80);
	setFlag('Z', value == 0);
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

void CPU::LSR(uint8_t* value) {
	uint8_t bitZero = (*value) & 0x01;
	setFlag('C', bitZero);

	(*value) >>= 1;
}

void CPU::OP_4A() {
	LSR(&aReg);
	pc += 1;
}

void CPU::OP_46NN() {
	uint8_t* value = zeroPagePtr();
	
	LSR(value);
	pc += 2;
}

void CPU::OP_56NN() {
	uint8_t* value = zeroPageXPtr();

	LSR(value);
	pc += 2;
}

void CPU::OP_4ENN00() {
	uint8_t* value = absolutePtr();

	LSR(value);
	pc += 3;
}

void CPU::OP_5ENN00() {
	uint8_t* value = absoluteXPtr();

	LSR(value);
	pc += 3;
}

void CPU::OP_EA() {}

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

void CPU::ROL(uint8_t* value) {
	uint8_t carryFlag = getFlag('C');
	uint8_t bit7 = (*value) & 0x80;

	setFlag('C', bit7);

	(*value) <<= 1;
	(*value) |= carryFlag;

	setFlag('N', (*value) & 0x80);
	setFlag('Z', (*value) == 0);
}

void CPU::OP_2A() {
	ROL(&aReg);
	pc += 1;
}

void CPU::OP_26NN() {
	uint8_t* value = zeroPagePtr();
	ROL(value);

	pc += 2;
}

void CPU::OP_36NN() {
	uint8_t* value = zeroPageXPtr();
	ROL(value);

	pc += 2;
}

void CPU::OP_2ENN00() {
	uint8_t* value = absolutePtr();
	ROL(value);

	pc += 3;
}

void CPU::OP_3ENN00() {
	uint8_t* value = absoluteXPtr();
	ROL(value);

	pc += 3;
}

void CPU::ROR(uint8_t* value) {
	uint8_t bitZero = (*value) & 0x1;
	uint8_t carryFlag = getFlag('C');

	(*value) >>= 1;
	(*value) |= (carryFlag << 7);

	setFlag('C', bitZero);
	setFlag('N', (*value) & 0x80);
	setFlag('Z', (*value) == 0);
}

void CPU::OP_6A() {
	ROR(&aReg);
	pc += 1;
}

void CPU::OP_66NN() {
	uint8_t* value = zeroPagePtr();
	ROR(value);

	pc += 2;
}

void CPU::OP_76NN() {
	uint8_t* value = zeroPageXPtr();
	ROR(value);

	pc += 2;
}

void CPU::OP_6ENN00() {
	uint8_t* value = absolutePtr();
	ROR(value);

	pc += 3;
}

void CPU::OP_7ENN00() {
	uint8_t* value = absoluteXPtr();
	ROR(value);

	pc += 3;
}

void CPU::OP_40() {
	status = pop();

	uint8_t pcLowByte = pop();
	uint8_t pcHighByte = pop();

	uint16_t pcAddress = (pcHighByte << 8) | pcLowByte;
	pc = pcAddress;
}

void CPU::OP_60() {
	uint8_t pcLowByte = pop();
	uint8_t pcHighByte = pop();

	uint16_t pcAddress = (pcHighByte << 8) | pcLowByte;
	pc = pcAddress;
}

void CPU::SBC(uint8_t value) {
	uint8_t reverseCarry = !getFlag('C');
	int16_t result = aReg - value - reverseCarry;

	setFlag('C', result <= 0xFF);
	setFlag('Z', result == 0);
	setFlag('N', result < 0);
	
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

void CPU::STA(uint8_t* value) {
	*value = aReg;
}

void CPU::OP_85NN() {
	uint8_t* value = zeroPagePtr();
	STA(value);

	pc += 2;
}

void CPU::OP_95NN() {
	uint8_t* value = zeroPageXPtr();
	STA(value);

	pc += 2;
}

void CPU::OP_8DNN00() {
	uint8_t* value = absolutePtr();
	STA(value);

	pc += 3;
}

void CPU::OP_9DNN00() {
	uint8_t* value = absoluteXPtr();
	STA(value);

	pc += 3;
}

void CPU::OP_99NN00() {
	uint8_t* value = absoluteYPtr();
	STA(value);

	pc += 3;
}

void CPU::OP_81NN() {
	uint8_t* value = indirectXPtr();
	STA(value);

	pc += 2;
}

void CPU::OP_91NN() {
	uint8_t* value = indirectYPtr();
	STA(value);

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
}

void CPU::OP_68() {
	aReg = pop();

	setFlag('N', aReg & 0x80);
	setFlag('Z', aReg == 0);

	pc += 1;
}

void CPU::OP_08() {
	push(status);
	pc += 1;
}

void CPU::OP_28() {
	status = pop();
	pc += 1;
}

void CPU::STX(uint8_t* value) {
	*value = xReg;
}

void CPU::OP_86NN() {
	uint8_t* value = zeroPagePtr();
	STX(value);

	pc += 2;
}

void CPU::OP_96NN() {
	uint8_t* value = zeroPageYPtr();
	STX(value);

	pc += 2;
}

void CPU::OP_8ENN00() {
	uint8_t* value = absolutePtr();
	STX(value);

	pc += 3;
}

void CPU::STY(uint8_t* value) {
	*value = yReg;
}

void CPU::OP_84NN() {
	uint8_t* value = zeroPagePtr();
	STY(value);

	pc += 2;
}

void CPU::OP_94NN() {
	uint8_t* value = zeroPageXPtr();
	STY(value);

	pc += 2;
}

void CPU::OP_8CNN00() {
	uint8_t* value = absolutePtr();
	STY(value);

	pc += 3;
}