#include <stdint.h>
#include "CPU.h"

uint8_t CPU::immediate() {
	uint8_t value = readMemory(pc + 1);
	return value;
}

uint8_t* CPU::immediatePtr() {
	uint8_t* value = &(memory[pc + 1]);
	return value;
}

uint8_t CPU::zeroPage() {
	cycles++;

	uint8_t address = readMemory(pc + 1);
	uint8_t value = readMemory(address);

	return value;
}

uint16_t CPU::zeroPagePtr() {
	cycles++;

	uint16_t address = memory[pc + 1];
	return address;
}

uint8_t CPU::zeroPageX() {
	cycles += 2;

	uint8_t address = readMemory(pc + 1);
	uint8_t value = readMemory((address + xReg) & 0xFF);

	return value;
}

uint16_t CPU::zeroPageXPtr() {
	cycles += 2;

	uint8_t address = memory[pc + 1];
	return (address + xReg) & 0xFF;
}

uint8_t CPU::zeroPageY() {
	cycles += 2;

	uint8_t address = readMemory(pc + 1);
	uint8_t value = readMemory((address + yReg) & 0xFF);

	return value;
}

uint16_t CPU::zeroPageYPtr() {
	cycles += 2;

	uint8_t address = memory[pc + 1];
	//uint8_t* value = &(memory[(address + yReg) & 0xFF]);

	return (address + yReg) & 0xFF;
}

uint8_t CPU::absolute() {
	cycles += 2;

	uint8_t lowByte = readMemory(pc + 1);
	uint8_t highByte = readMemory(pc + 2);

	uint16_t address = (highByte << 8) | lowByte;
	uint8_t value = readMemory(address);

	return value;
}

uint16_t CPU::absolutePtr() {
	cycles += 2;

	uint8_t lowByte = readMemory(pc + 1);
	uint8_t highByte = readMemory(pc + 2);

	uint16_t address = (highByte << 8) | lowByte;

	return address;
}

uint8_t CPU::absoluteX() {
	cycles += 2;

	uint8_t lowByte = readMemory(pc + 1);
	uint8_t highByte = readMemory(pc + 2);

	uint16_t address = (highByte << 8) | lowByte;
	uint8_t value = readMemory((address + xReg) & 0xFFFF);

	if ((lowByte + xReg) > 0xFF)
	{
		cycles++;
	}

	return value;
}

uint16_t CPU::absoluteXPtr() {
	cycles += 2;

	uint8_t lowByte = readMemory(pc + 1);
	uint8_t highByte = readMemory(pc + 2);

	uint16_t address = (highByte << 8) | lowByte;

	if ((lowByte + xReg) > 0xFF)
	{
		cycles++;
	}

	return (address + xReg) & 0xFFFF;
}

uint8_t CPU::absoluteY() {
	cycles += 2;

	uint8_t lowByte = readMemory(pc + 1);
	uint8_t highByte = readMemory(pc + 2);

	uint16_t address = (highByte << 8) | lowByte;
	uint8_t value = readMemory((address + yReg) & 0xFFFF);

	if ((lowByte + yReg) > 0xFF)
	{
		cycles++;
	}

	return value;
}

uint16_t CPU::absoluteYPtr() {
	cycles += 2;

	uint8_t lowByte = readMemory(pc + 1);
	uint8_t highByte = readMemory(pc + 2);

	uint16_t address = (highByte << 8) | lowByte;
	//uint8_t* value = &(memory[(address + yReg) & 0xFFFF]);

	if ((lowByte + yReg) > 0xFF)
	{
		cycles++;
	}

	return (address + yReg) & 0xFFFF;
}

uint8_t CPU::indirectX() {
	cycles += 4;

	uint8_t addressPtr = (readMemory(pc + 1) + xReg) & 0xFF;

	uint8_t lowByte = readMemory(addressPtr);
	uint8_t highByte = readMemory((addressPtr + 1) & 0xFF);

	uint16_t address = (highByte << 8) | lowByte;
	uint8_t value = readMemory(address);

	return value;
}

uint16_t CPU::indirectXPtr() {
	cycles += 4;

	uint8_t addressPtr = (readMemory(pc + 1) + xReg) & 0xFF;

	uint8_t lowByte = readMemory(addressPtr);
	uint8_t highByte = readMemory((addressPtr + 1) & 0xFF);

	uint16_t address = (highByte << 8) | lowByte;

	return address;
}

uint8_t CPU::indirectY() {
	cycles += 3;

	uint8_t addressPtr = readMemory(pc + 1);

	uint8_t lowByte = readMemory(addressPtr);
	uint8_t highByte = readMemory((addressPtr + 1) & 0xFF);

	uint16_t address = ((highByte << 8) | lowByte);
	uint8_t value = readMemory((address + yReg) & 0xFFFF);

	if ((lowByte + yReg) > 0xFF)
	{
		cycles++;
	}

	return value;
}

uint16_t CPU::indirectYPtr() {
	cycles += 3;

	uint8_t addressPtr = readMemory(pc + 1);

	uint8_t lowByte = readMemory(addressPtr);
	uint8_t highByte = readMemory((addressPtr + 1) & 0xFF);

	uint16_t address = ((highByte << 8) | lowByte);
	//uint8_t* value = &(memory[(address + yReg) & 0xFFFF]);

	if ((lowByte + yReg) > 0xFF)
	{
		cycles++;
	}

	return (address + yReg) & 0xFFFF;
}

