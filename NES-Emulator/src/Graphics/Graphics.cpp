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
	colorsMapTable[0x0] = 0x626262;
	colorsMapTable[0x1] = 0x01208f;
	colorsMapTable[0x2] = 0x230b9e;
	colorsMapTable[0x3] = 0x480090;
	colorsMapTable[0x4] = 0x600062;
	colorsMapTable[0x5] = 0x6a0124;
	colorsMapTable[0x6] = 0x601200;
	colorsMapTable[0x7] = 0x472700;
	colorsMapTable[0x8] = 0x243c00;
	colorsMapTable[0x9] = 0x034a00;
	colorsMapTable[0xa] = 0x004f00;
	colorsMapTable[0xb] = 0x004824;
	colorsMapTable[0xc] = 0x003662;
	colorsMapTable[0xd] = 0x000000;
	colorsMapTable[0xe] = 0x000000;
	colorsMapTable[0xf] = 0x000000;
	colorsMapTable[0x10] = 0xababab;
	colorsMapTable[0x11] = 0x1e56e1;
	colorsMapTable[0x12] = 0x4e39ff;
	colorsMapTable[0x13] = 0x7e22ef;
	colorsMapTable[0x14] = 0xa21bb6;
	colorsMapTable[0x15] = 0xb42263;
	colorsMapTable[0x16] = 0xac360e;
	colorsMapTable[0x17] = 0x8c5600;
	colorsMapTable[0x18] = 0x5e7200;
	colorsMapTable[0x19] = 0x2d8800;
	colorsMapTable[0x1a] = 0x079100;
	colorsMapTable[0x1b] = 0x018947;
	colorsMapTable[0x1c] = 0x00739c;
	colorsMapTable[0x1d] = 0x000000;
	colorsMapTable[0x1e] = 0x000000;
	colorsMapTable[0x1f] = 0x000000;
	colorsMapTable[0x20] = 0xffffff;
	colorsMapTable[0x21] = 0x68acff;
	colorsMapTable[0x22] = 0x958cff;
	colorsMapTable[0x23] = 0xc876ff;
	colorsMapTable[0x24] = 0xf36aff;
	colorsMapTable[0x25] = 0xff6fc6;
	colorsMapTable[0x26] = 0xff836b;
	colorsMapTable[0x27] = 0xe5a01f;
	colorsMapTable[0x28] = 0xb8bf00;
	colorsMapTable[0x29] = 0x85d802;
	colorsMapTable[0x2a] = 0x5ae235;
	colorsMapTable[0x2b] = 0x44de88;
	colorsMapTable[0x2c] = 0x48cae2;
	colorsMapTable[0x2d] = 0x4e4e4e;
	colorsMapTable[0x2e] = 0x000000;
	colorsMapTable[0x2f] = 0x000000;
	colorsMapTable[0x30] = 0xffffff;
	colorsMapTable[0x31] = 0xbfe0ff;
	colorsMapTable[0x32] = 0xd0d3fe;
	colorsMapTable[0x33] = 0xe6c9ff;
	colorsMapTable[0x34] = 0xf6c3fe;
	colorsMapTable[0x35] = 0xfec4ed;
	colorsMapTable[0x36] = 0xfecbc8;
	colorsMapTable[0x37] = 0xf7d6a9;
	colorsMapTable[0x38] = 0xe6e396;
	colorsMapTable[0x39] = 0xd1ed96;
	colorsMapTable[0x3a] = 0xbff3a9;
	colorsMapTable[0x3b] = 0xb5f2c9;
	colorsMapTable[0x3c] = 0xb5eaee;
	colorsMapTable[0x3d] = 0xb8b8b8;
	colorsMapTable[0x3e] = 0x000000;
	colorsMapTable[0x3f] = 0x000000;
}