#pragma once

void initialize(int windowWidth, int windowHeight, int textureWidth, int textureHeight);
void update(const void* buffer, int pitch);
void displayPatternTable(PPU ppu);
void shutdown();
