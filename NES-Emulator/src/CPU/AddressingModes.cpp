#include <stdint.h>
#include "CPU.h"

uint8_t CPU::immediate() {
	uint8_t value = memory[pc + 1];
	return value;
}

uint8_t* CPU::immediatePtr() {
	uint8_t* value = &(memory[pc + 1]);
	return value;
}

uint8_t CPU::zeroPage() {
	cycles++;

	uint8_t address = memory[pc + 1];
	uint8_t value = memory[address];

	return value;
}

uint8_t* CPU::zeroPagePtr() {
	cycles++;

	uint8_t address = memory[pc + 1];
	uint8_t* value = &(memory[address]);

	return value;
}

uint8_t CPU::zeroPageX() {
	cycles += 2;

	uint8_t address = memory[pc + 1];
	uint8_t value = memory[(address + xReg) & 0xFF];

	return value;
}

uint8_t* CPU::zeroPageXPtr() {
	cycles += 2;

	uint8_t address = memory[pc + 1];
	uint8_t* value = &(memory[(address + xReg) & 0xFF]);

	return value;
}

uint8_t CPU::zeroPageY() {
	cycles += 2;

	uint8_t address = memory[pc + 1];
	uint8_t value = memory[(address + yReg) & 0xFF];

	return value;
}

uint8_t* CPU::zeroPageYPtr() {
	cycles += 2;

	uint8_t address = memory[pc + 1];
	uint8_t* value = &(memory[(address + yReg) & 0xFF]);

	return value;
}

uint8_t CPU::absolute() {
	cycles += 2;

	uint8_t lowByte = memory[pc + 1];
	uint8_t highByte = memory[pc + 2];

	uint16_t address = (highByte << 8) | lowByte;
	uint8_t value = memory[address];

	return value;
}

uint8_t* CPU::absolutePtr() {
	cycles += 2;

	uint8_t lowByte = memory[pc + 1];
	uint8_t highByte = memory[pc + 2];

	uint16_t address = (highByte << 8) | lowByte;
	uint8_t* value = &(memory[address]);

	return value;
}

uint8_t CPU::absoluteX() {
	cycles += 2;

	uint8_t lowByte = memory[pc + 1];
	uint8_t highByte = memory[pc + 2];

	uint16_t address = (highByte << 8) | lowByte;
	uint8_t value = memory[(address + xReg) & 0xFFFF];

	if ((lowByte + xReg) > 0xFF)
	{
		cycles++;
	}

	return value;
}

uint8_t* CPU::absoluteXPtr() {
	cycles += 2;

	uint8_t lowByte = memory[pc + 1];
	uint8_t highByte = memory[pc + 2];

	uint16_t address = (highByte << 8) | lowByte;
	uint8_t* value = &(memory[(address + xReg) & 0xFFFF]);

	if ((lowByte + xReg) > 0xFF)
	{
		cycles++;
	}

	return value;
}

uint8_t CPU::absoluteY() {
	cycles += 2;

	uint8_t lowByte = memory[pc + 1];
	uint8_t highByte = memory[pc + 2];

	uint16_t address = (highByte << 8) | lowByte;
	uint8_t value = memory[(address + yReg) & 0xFFFF];

	if ((lowByte + yReg) > 0xFF)
	{
		cycles++;
	}

	return value;
}

uint8_t* CPU::absoluteYPtr() {
	cycles += 2;

	uint8_t lowByte = memory[pc + 1];
	uint8_t highByte = memory[pc + 2];

	uint16_t address = (highByte << 8) | lowByte;
	uint8_t* value = &(memory[(address + yReg) & 0xFFFF]);

	if ((lowByte + yReg) > 0xFF)
	{
		cycles++;
	}

	return value;
}

uint8_t CPU::indirectX() {
	cycles += 4;

	uint8_t addressPtr = (memory[pc + 1] + xReg) & 0xFF;

	uint8_t lowByte = memory[addressPtr];
	uint8_t highByte = memory[(addressPtr + 1) & 0xFF];

	uint16_t address = (highByte << 8) | lowByte;
	uint8_t value = memory[address];

	return value;
}

uint8_t* CPU::indirectXPtr() {
	cycles += 4;

	uint8_t addressPtr = (memory[pc + 1] + xReg) & 0xFF;

	uint8_t lowByte = memory[addressPtr];
	uint8_t highByte = memory[(addressPtr + 1) & 0xFF];

	uint16_t address = (highByte << 8) | lowByte;
	uint8_t* value = &(memory[address]);

	return value;
}

uint8_t CPU::indirectY() {
	cycles += 3;

	uint8_t addressPtr = memory[pc + 1];

	uint8_t lowByte = memory[addressPtr];
	uint8_t highByte = memory[(addressPtr + 1) & 0xFF];

	uint16_t address = ((highByte << 8) | lowByte);
	uint8_t value = memory[(address + yReg) & 0xFFFF];

	if ((lowByte + yReg) > 0xFF)
	{
		cycles++;
	}

	return value;
}

uint8_t* CPU::indirectYPtr() {
	cycles += 3;

	uint8_t addressPtr = memory[pc + 1];

	uint8_t lowByte = memory[addressPtr];
	uint8_t highByte = memory[(addressPtr + 1) & 0xFF];

	uint16_t address = ((highByte << 8) | lowByte);
	uint8_t* value = &(memory[(address + yReg) & 0xFFFF]);

	if ((lowByte + yReg) > 0xFF)
	{
		cycles++;
	}

	return value;
}

