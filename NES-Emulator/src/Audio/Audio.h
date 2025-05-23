#pragma once
#include <stdint.h>
#include <vector>
#include <queue>
#include "SDL.h"

namespace Audio {
	constexpr int SAMPLE_RATE = 44100;
	constexpr int AMPLITUDE = 1;
	constexpr int BUFFER_SIZE = 735;
	constexpr int audioBufferSize = BUFFER_SIZE * sizeof(float);

	extern SDL_AudioDeviceID device;

	extern std::vector<float> soundBuffer;
	extern std::queue<float> soundQueue;
	extern int index;

	void initialize();
}

