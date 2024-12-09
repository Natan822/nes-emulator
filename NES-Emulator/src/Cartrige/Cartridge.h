#pragma once
#include <vector>

class Cartridge {
public:

	Cartridge(int prgSize, int chrSize);
	~Cartridge();

	std::vector<uint8_t> prg;
	std::vector<uint8_t> chr;

	void loadPrg(char* buffer);
	void loadChr(char* buffer);

	int prgSize{};
	int chrSize{};
};