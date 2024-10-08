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
	uint8_t address = memory[pc + 1];
	uint8_t value = memory[address];

	return value;
}

uint8_t* CPU::zeroPagePtr() {
	uint8_t address = memory[pc + 1];
	uint8_t* value = &(memory[address]);

	return value;
}

uint8_t CPU::zeroPageX() {
	uint8_t address = memory[pc + 1];
	uint8_t value = memory[address + xReg];

	return value;
}

uint8_t* CPU::zeroPageXPtr() {
	uint8_t address = memory[pc + 1];
	uint8_t* value = &(memory[address + xReg]);

	return value;
}

uint8_t CPU::zeroPageY() {
	uint8_t address = memory[pc + 1];
	uint8_t value = memory[address + yReg];

	return value;
}

uint8_t* CPU::zeroPageYPtr() {
	uint8_t address = memory[pc + 1];
	uint8_t* value = &(memory[address + yReg]);

	return value;
}

uint8_t CPU::absolute() {
	uint8_t lowByte = memory[pc + 1];
	uint8_t highByte = memory[pc + 2];

	uint16_t address = (highByte << 8) | lowByte;
	uint8_t value = memory[address];

	return value;
}

uint8_t* CPU::absolutePtr() {
	uint8_t lowByte = memory[pc + 1];
	uint8_t highByte = memory[pc + 2];

	uint16_t address = (highByte << 8) | lowByte;
	uint8_t* value = &(memory[address]);

	return value;
}

uint8_t CPU::absoluteX() {
	uint8_t lowByte = memory[pc + 1];
	uint8_t highByte = memory[pc + 2];

	uint16_t address = (highByte << 8) | lowByte;
	uint8_t value = memory[address + xReg];

	return value;
}

uint8_t* CPU::absoluteXPtr() {
	uint8_t lowByte = memory[pc + 1];
	uint8_t highByte = memory[pc + 2];

	uint16_t address = (highByte << 8) | lowByte;
	uint8_t* value = &(memory[address + xReg]);

	return value;
}

uint8_t CPU::absoluteY() {
	uint8_t lowByte = memory[pc + 1];
	uint8_t highByte = memory[pc + 2];

	uint16_t address = (highByte << 8) | lowByte;
	uint8_t value = memory[address + yReg];

	return value;
}

uint8_t* CPU::absoluteYPtr() {
	uint8_t lowByte = memory[pc + 1];
	uint8_t highByte = memory[pc + 2];

	uint16_t address = (highByte << 8) | lowByte;
	uint8_t* value = &(memory[address + yReg]);

	return value;
}

uint8_t CPU::indirectX() {
	uint8_t addressPtr = (memory[pc + 1] + xReg) & 0xFF;

	uint8_t lowByte = memory[addressPtr];
	uint8_t highByte = memory[(addressPtr + 1) & 0xFF];

	uint16_t address = (highByte << 8) | lowByte;
	uint8_t value = memory[address];

	return value;
}

uint8_t* CPU::indirectXPtr() {
	uint8_t addressPtr = (memory[pc + 1] + xReg) & 0xFF;

	uint8_t lowByte = memory[addressPtr];
	uint8_t highByte = memory[(addressPtr + 1) & 0xFF];

	uint16_t address = (highByte << 8) | lowByte;
	uint8_t* value = &(memory[address]);

	return value;
}

uint8_t CPU::indirectY() {
	uint8_t addressPtr = memory[pc + 1];

	uint8_t lowByte = memory[addressPtr];
	uint8_t highByte = memory[(addressPtr + 1) & 0xFF];

	uint16_t address = ((highByte << 8) | lowByte);
	uint8_t value = memory[(address + yReg) & 0xFFFF];

	return value;
}

uint8_t* CPU::indirectYPtr() {
	uint8_t addressPtr = memory[pc + 1];

	uint8_t lowByte = memory[addressPtr];
	uint8_t highByte = memory[(addressPtr + 1) & 0xFF];

	uint16_t address = ((highByte << 8) | lowByte);
	uint8_t* value = &(memory[(address + yReg) & 0xFFFF]);

	return value;
}

