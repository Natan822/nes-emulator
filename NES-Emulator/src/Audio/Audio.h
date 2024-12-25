#pragma once
#include <stdint.h>
#include "SDL.h"

namespace Audio {
	constexpr int SAMPLE_RATE = 44100;
	constexpr int AMPLITUDE = 28000;
	constexpr int FREQUENCY = 440;
	constexpr float DURATION = 0.1f;
	constexpr int BUFFER_SIZE = 1024;
	constexpr int audioBufferSize = SAMPLE_RATE * sizeof(uint8_t);

	extern SDL_AudioDeviceID device;

	void initialize();
}