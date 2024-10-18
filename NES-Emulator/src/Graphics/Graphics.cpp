#include "SDL.h"
#include "../PPU/PPU.h"
#include "Graphics.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;

void initialize(int windowWidth, int windowHeight, int textureWidth, int textureHeight) {
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("NES Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
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