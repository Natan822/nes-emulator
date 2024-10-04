#pragma once
class CPU {

	const int CARTRIDGE_START_ADDRESS = 0x4018u;
	const int STACK_START_ADDRESS = 0x1FF;

	const int PRG_START_ADDRESS = 0x8000;

	typedef void(CPU::* OPCODE)();

public:
	// Registers
	uint8_t aReg{}; // A
	uint8_t xReg{}; // X
	uint8_t yReg{}; // Y
	uint16_t pc{}; // Program Counter
	uint8_t sp{}; // Stack pointer)
	uint8_t status{}; // Each bit is a flag. The flags from left to right are: 
	/*
		7 = Negative
		6 = Overflow
		5 = No CPU effect; always pushed as 1
		4 = No CPU effect; B flag
		3 = Decimal
		2 = Interrupt Disable
		1 = Zero
		0 = Carry
	*/

	// Memory
	uint8_t memory[0xFFFF + 1]{};

	uint8_t prgSize{}; // PRG ROM size in 16 KiB units
	uint8_t chrSize{}; // CHR ROM size in 8 KiB units(0 = board uses CHR RAM)

	CPU();
	~CPU();

	// Table of opcodes
	OPCODE table[0xFF]{};

	void loadROM(std::string filePath);
	void cycle();

private:

	void execute();

	void push(uint8_t value);
	uint8_t pop();

	void setFlag(char flag, uint8_t value);
	bool getFlag(char flag);

	// Instructions Addressing Modes

	uint8_t immediate();
	uint8_t* immediatePtr();

	uint8_t zeroPage();
	uint8_t* zeroPagePtr();

	uint8_t zeroPageX();
	uint8_t* zeroPageXPtr();

	uint8_t zeroPageY();
	uint8_t* zeroPageYPtr();

	uint8_t absolute();
	uint8_t* absolutePtr();

	uint8_t absoluteX();
	uint8_t* absoluteXPtr();

	uint8_t absoluteY();
	uint8_t* absoluteYPtr();

	uint8_t indirectX();
	uint8_t* indirectXPtr();

	uint8_t indirectY();
	uint8_t* indirectYPtr();

	// Instructions
	// ADC
	void ADC(uint8_t value); // General ADC instruction
	void OP_69NN(); // Immediate
	void OP_65NN(); // Zero Page
	void OP_75NN(); // Zero Page, X
	void OP_6DNN00(); // Absolute
	void OP_7DNN00(); // Absolute, X
	void OP_79NN00(); // Absolute, Y
	void OP_61NN(); // Indirect, X
	void OP_71NN(); // Indirect, Y

	// AND
	void AND(uint8_t value); // General AND instruction
	void OP_29NN(); // Immediate
	void OP_25NN(); // Zero Page
	void OP_35NN(); // Zero Page, X
	void OP_2DNN00(); // Absolute
	void OP_3DNN00(); // Absolute, X
	void OP_39NN00(); // Absolute, Y
	void OP_21NN(); // Indirect, X
	void OP_31NN(); // Indirect, Y

	// ASL
	void ASL(uint8_t* value); // General ASL instruction
	void OP_0A(); // Accumulator
	void OP_06NN(); // Zero Page
	void OP_16NN(); // Zero Page, X
	void OP_0ENN00(); // Absolute
	void OP_1ENN00(); // Absolute, X

	// BIT
	void BIT(uint8_t value); // General BIT instruction
	void OP_24NN(); // Zero Page
	void OP_2CNN00(); // Absolute

	// BRANCH
	void BRANCH(uint8_t value); // General BRANCH instruction
	void OP_90NN(); // Branch on carry = 0
	void OP_B0NN(); // Branch on carry = 1
	void OP_F0NN(); // Branch on zero = 1
	void OP_30NN(); // Branch on negative = 1
	void OP_D0NN(); // Branch on zero = 0
	void OP_10NN(); // Branch on negative = 0
	void OP_50NN(); // Branch on overflow = 0
	void OP_70NN(); // Branch on overflow = 1

	// BRK
	void OP_00NN();

	// CMP(If accumulator >= compared value, Carry = 1. Z and N flags will be set based on the result of accumulator - value)
	void CMP(uint8_t value); // General CMP instruction
	void OP_C9NN(); // Immediate
	void OP_C5NN(); // Zero Page
	void OP_D5NN(); // Zero Page, X
	void OP_CDNN00(); // Absolute
	void OP_DDNN00(); // Absolute, X
	void OP_D9NN00(); // Absolute, Y
	void OP_C1NN(); // Indirect, X
	void OP_D1NN(); // Indirect, Y
	
	// CPX(Same as CMP but uses X register instead of accumulator)
	void CPX(uint8_t value); // General CPX instruction
	void OP_E0NN(); // Immediate
	void OP_E4NN(); // Zero Page
	void OP_ECNN00(); // Absolute	

	// CPY(Same as CMP but uses Y register instead of accumulator)
	void CPY(uint8_t value); // General CPX instruction
	void OP_C0NN(); // Immediate
	void OP_C4NN(); // Zero Page
	void OP_CCNN00(); // Absolute

	// DEC(decrement memory)
	void DEC(uint8_t* value); // General DEC instruction
	void OP_C6NN(); // Zero Page
	void OP_D6NN(); // Zero Page, X
	void OP_CENN00(); // Absolute
	void OP_DENN00(); // Absolute, X

	//EOR(exclusive OR)
	void EOR(uint8_t value); // General EOR instruction
	void OP_49NN(); // Immediate
	void OP_45NN(); // Zero Page
	void OP_55NN(); // Zero Page, X
	void OP_4DNN00(); // Absolute
	void OP_5DNN00(); // Absolute, X
	void OP_59NN00(); // Absolute, Y
	void OP_41NN(); // Indirect, X
	void OP_51NN(); // Indirect, Y

	// Flag instructions
	void OP_18(); // CLC(Clear Carry)    
	void OP_38(); // SEC(Set Carry)      
	void OP_58(); // CLI(Clear Interrupt)
	void OP_78(); // SEI(Set Interrupt)  
	void OP_B8(); // CLV(Clear oVerflow) 
	void OP_D8(); // CLD(Clear Decimal)  
	void OP_F8(); // SED(Set Decimal)    

	// INC(increment memory)
	void INC(uint8_t* value); // General INC instruction
	void OP_E6NN(); // Zero Page
	void OP_F6NN(); // Zero Page, X
	void OP_EENN00(); // Absolute
	void OP_FENN00(); // Absolute, X

	// JMP
	void JMP(uint16_t address); // General JMP instruction
	void OP_4CNN00(); // Absolute
	void OP_6CNN00(); // Indirect

	// JSR(same as JMP but save return address in stack)
	void OP_20NN00(); // Absolute

	// LDA(load accumulator)
	void LDA(uint8_t value); // General LDA instruction
	void OP_A9NN(); // Immediate
	void OP_A5NN(); // Zero Page
	void OP_B5NN(); // Zero Page, X
	void OP_ADNN00(); // Absolute
	void OP_BDNN00(); // Absolute, X
	void OP_B9NN00(); // Absolute, Y
	void OP_A1NN(); // Indirect, X
	void OP_B1NN(); // Indirect, Y

	// LDX(load X register)
	void LDX(uint8_t value); // General LDX instruction
	void OP_A2NN(); // Immediate
	void OP_A6NN(); // Zero Page
	void OP_B6NN(); // Zero Page, Y
	void OP_AENN00(); // Absolute
	void OP_BENN00(); // Absolute, Y

	// LDY(load Y register)
	void LDY(uint8_t value); // General LDY instruction
	void OP_A0NN(); // Immediate
	void OP_A4NN(); // Zero Page
	void OP_B4NN(); // Zero Page, X
	void OP_ACNN00(); // Absolute
	void OP_BCNN00(); // Absolute, X

	// LSR(shift right one position and original bit 0 is stored at carry)
	void LSR(uint8_t* value);
	void OP_4A(); // Accumulator
	void OP_46NN(); // Zero Page
	void OP_56NN(); // Zero Page, X
	void OP_4ENN00(); // Absolute
	void OP_5ENN00(); // Absolute, X

	// NOP
	void OP_EA();

	// ORA(OR with accumulator)
	void ORA(uint8_t value); // General ORA instruction
	void OP_09NN(); // Immediate
	void OP_05NN(); // Zero Page
	void OP_15NN(); // Zero Page, X
	void OP_0DNN00(); // Absolute
	void OP_1DNN00(); // Absolute, X
	void OP_19NN00(); // Absolute, Y
	void OP_01NN(); // Indirect, X
	void OP_11NN(); // Indirect, Y

	// Register instructions
	void OP_AA(); // TAX(Transfer A to X)
	void OP_8A(); // TXA(Transfer X to A)
	void OP_CA(); // DEX(Decrement X)    
	void OP_E8(); // INX(Increment X)    
	void OP_A8(); // TAY(Transfer A to Y)
	void OP_98(); // TYA(Transfer Y to A)
	void OP_88(); // DEY(Decrement Y)    
	void OP_C8(); // INY(Increment Y)    

	// ROL(shifts all bits left, bit 0 = carry, carry = original bit 7)
	void ROL(uint8_t* value); // General ROL instruction
	void OP_2A(); // Accumulator  
	void OP_26NN(); // Zero Page    
	void OP_36NN(); // Zero Page, X
	void OP_2ENN00(); // Absolute 
	void OP_3ENN00(); // Absolute, X   

	// ROR(shifts all bits right, bit 7 = carry, carry = original bit 0)
	void ROR(uint8_t* value); // General ROR instruction
	void OP_6A(); // Accumulator	
	void OP_66NN(); // Zero Page	
	void OP_76NN(); // Zero Page, X	
	void OP_6ENN00(); // Absolute	
	void OP_7ENN00(); // Absolute, X

	// RTI(return from interrupt)
	void OP_40(); // retrieves the status and the pc from the stack in that order

	// RTS(return from subroutine)
	void OP_60();

	// SBC(subtract with carry)
	void SBC(uint8_t value); // General SBC instruction
	void OP_E9NN(); // Immediate
	void OP_E5NN();	// Zero Page
	void OP_F5NN();	// Zero Page, X
	void OP_EDNN00(); // Absolute
	void OP_FDNN00(); // Absolute, X
	void OP_F9NN00(); // Absolute, Y
	void OP_E1NN(); // Indirect, X
	void OP_F1NN(); // Indirect, Y

	// STA(Store accumulator in memory)
	void STA(uint8_t* value); // General STA instruction
	void OP_85NN(); // Zero Page
	void OP_95NN(); // Zero Page, X
	void OP_8DNN00(); // Absolute
	void OP_9DNN00(); // Absolute, X
	void OP_99NN00(); // Absolute, Y
	void OP_81NN(); // Indirect, X
	void OP_91NN(); // Indirect, Y

	// Stack instructions
	void OP_9A(); // TXS(Transfer X to Stack ptr)
	void OP_BA(); // TSX(Transfer Stack ptr to X)
	void OP_48(); // PHA(Push accumulator)       
	void OP_68(); // PLA(Pull accumulator)       
	void OP_08(); // PHP(Push processor status)  
	void OP_28(); // PLP(Pull processor status)  

	// STX(Store X in memory)
	void STX(uint8_t* value); // General STX instruction
	void OP_86NN(); // Zero Page
	void OP_96NN(); // Zero Page, Y
	void OP_8ENN00(); // Absolute

	// STY(Store Y in memory)
	void STY(uint8_t* value); // General STY instruction
	void OP_84NN(); // Zero Page
	void OP_94NN(); // Zero Page, X
	void OP_8CNN00(); // Absolute


};