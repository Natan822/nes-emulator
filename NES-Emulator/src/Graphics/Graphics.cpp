#include "SDL.h"
#include "../PPU/PPU.h"
#include "Graphics.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;

int colorsMapTable[0x3F + 1];

void initialize(int windowWidth, int windowHeight, int textureWidth, int textureHeight) {
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("NES Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);

	loadMapTable();
}

void update(const void* buffer, int pitch) {
	SDL_UpdateTexture(texture, NULL, buffer, pitch);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void displayPatternTable(PPU ppu) {
	ppu.loadPatternTable();
	update(ppu.video, (sizeof(ppu.video[0]) * VIDEO_WIDTH));
}

void shutdown() {
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void loadMapTable() {
	colorsMapTable[0x0] = 0x4B5054;
	colorsMapTable[0x1] = 0x011A74;
	colorsMapTable[0x2] = 0x1D0A80;
	colorsMapTable[0x3] = 0x390073;
	colorsMapTable[0x4] = 0x4D004E;
	colorsMapTable[0x5] = 0x55001D;
	colorsMapTable[0x6] = 0x4D0E00;
	colorsMapTable[0x7] = 0x391E00;
	colorsMapTable[0x8] = 0x1D3100;
	colorsMapTable[0x9] = 0x013B00;
	colorsMapTable[0xa] = 0x013F00;
	colorsMapTable[0xb] = 0x01391E;
	colorsMapTable[0xc] = 0x002B4E;
	colorsMapTable[0xd] = 0x000000;
	colorsMapTable[0xe] = 0x000000;
	colorsMapTable[0xf] = 0x000000;
	colorsMapTable[0x10] = 0x484D51;
	colorsMapTable[0x11] = 0x1843B3;
	colorsMapTable[0x12] = 0x1100A9;
	colorsMapTable[0x13] = 0x560DB4;
	colorsMapTable[0x14] = 0x821592;
	colorsMapTable[0x15] = 0x901B50;
	colorsMapTable[0x16] = 0x8A2B0B;
	colorsMapTable[0x17] = 0x714301;
	colorsMapTable[0x18] = 0x4B5C00;
	colorsMapTable[0x19] = 0x246D01;
	colorsMapTable[0x1a] = 0x077301;
	colorsMapTable[0x1b] = 0x006E39;
	colorsMapTable[0x1c] = 0x005C7E;
	colorsMapTable[0x1d] = 0x000000;
	colorsMapTable[0x1e] = 0x000000;
	colorsMapTable[0x1f] = 0x000000;
	colorsMapTable[0x20] = 0x191A1C;
	colorsMapTable[0x21] = 0x00418D;
	colorsMapTable[0x22] = 0x080077;
	colorsMapTable[0x23] = 0x500087;
	colorsMapTable[0x24] = 0x80008B;
	colorsMapTable[0x25] = 0x890052;
	colorsMapTable[0x26] = 0x8C1800;
	colorsMapTable[0x27] = 0xB37C15;
	colorsMapTable[0x28] = 0x939901;
	colorsMapTable[0x29] = 0x69AD00;
	colorsMapTable[0x2a] = 0x53A618;
	colorsMapTable[0x2b] = 0x1B9C66;
	colorsMapTable[0x2c] = 0x178499;
	colorsMapTable[0x2d] = 0x3B4043;
	colorsMapTable[0x2e] = 0x000000;
	colorsMapTable[0x2f] = 0x000000;
	colorsMapTable[0x30] = 0x191A1C;
	colorsMapTable[0x31] = 0x2A2E2F;
	colorsMapTable[0x32] = 0x24282B;
	colorsMapTable[0x33] = 0x272B2C;
	colorsMapTable[0x34] = 0x4A0057;
	colorsMapTable[0x35] = 0x56003D;
	colorsMapTable[0x36] = 0x530402;
	colorsMapTable[0x37] = 0x623F0A;
	colorsMapTable[0x38] = 0x4E4C13;
	colorsMapTable[0x39] = 0x3F500E;
	colorsMapTable[0x3a] = 0x36600E;
	colorsMapTable[0x3b] = 0x115833;
	colorsMapTable[0x3c] = 0x155255;
	colorsMapTable[0x3d] = 0x404548;
	colorsMapTable[0x3e] = 0x000000;
	colorsMapTable[0x3f] = 0x000000;
}