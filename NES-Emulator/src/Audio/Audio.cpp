#include "SDL.h"
#include "Audio.h"

namespace Audio {
	SDL_AudioDeviceID device;

	void initialize() {
		SDL_AudioSpec desiredSpec;

		desiredSpec.freq = SAMPLE_RATE;
		desiredSpec.format = AUDIO_S8;
		desiredSpec.channels = 1;
		desiredSpec.samples = BUFFER_SIZE; 
		desiredSpec.callback = nullptr;

		device = SDL_OpenAudioDevice(nullptr, 0, &desiredSpec, NULL, 0);
		SDL_PauseAudioDevice(device, 0);
	}
}