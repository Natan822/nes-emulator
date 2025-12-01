#pragma once
#include <stdint.h>
#include <vector>
#include <queue>
#include <SDL3/SDL.h>

namespace Audio {
	constexpr int SAMPLE_RATE = 44100;
	constexpr int AMPLITUDE = 1;
	constexpr int BUFFER_SIZE = 735;
	constexpr int audioBufferSize = BUFFER_SIZE * sizeof(float);

	extern SDL_AudioDeviceID device;
	extern SDL_AudioStream* stream;

	extern std::vector<float> soundBuffer;
	extern std::queue<float> soundQueue;
	extern int index;

	void init();
	void initAudioStream(const SDL_AudioSpec* srcSpec, const SDL_AudioSpec* destSpec);
}

