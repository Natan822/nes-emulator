#include <SDL3/SDL.h>
#include "Audio.h"
#include "../App.h"
#include "../APU/APU.h"
#include <iostream>
#include <vector>
#include <queue>

namespace Audio
{
	SDL_AudioDeviceID device;
	SDL_AudioStream* stream;

	std::vector<float> soundBuffer(BUFFER_SIZE);
	std::queue<float> soundQueue;
	int index = 0;

	// Unused audio callback
	void audioCallback(void *userdata, Uint8 *stream, int len)
	{
		float *audioStream = reinterpret_cast<float *>(stream);

		const int samplesGiven = soundQueue.size();
		const int expectedSamples = BUFFER_SIZE;

		if (soundQueue.size() < expectedSamples)
		{
			std::cout << "audioCallback: not enough samples\n";
			std::cout << "missing: " << expectedSamples - samplesGiven << "samples\n";
			return;
		}

		for (size_t i = 0; i < expectedSamples; i++)
		{
			if (soundQueue.empty())
			{
				break;
			}
			else
			{
				audioStream[i] = soundQueue.front();
				soundQueue.pop();
			}
		}
	}

	void init()
	{
		SDL_AudioSpec desiredSpec;

		desiredSpec.freq = SAMPLE_RATE;
		desiredSpec.format = SDL_AUDIO_F32LE;
		desiredSpec.channels = 1;
		/* SDL2
		 desiredSpec.samples = BUFFER_SIZE;
		 desiredSpec.callback = nullptr;
	     device = SDL_OpenAudioDevice(nullptr, 0, &desiredSpec, NULL, 0);
		*/
		device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desiredSpec);
		if (device == 0) {
			std::cerr << "ERROR (Audio): Failed to open SDL_AudioDeviceID\n";
			std::cerr << "SDL: " << SDL_GetError() << std::endl;
			return;
		}

		initAudioStream(&desiredSpec, &desiredSpec);
	}

	void initAudioStream(const SDL_AudioSpec* srcSpec, const SDL_AudioSpec* destSpec) {
		stream = SDL_CreateAudioStream(srcSpec, destSpec);
		if (stream == NULL) {
			std::cerr << "ERROR (Audio): Failed to create SDL_AudioStream\n";
			std::cerr << "SDL: " << SDL_GetError() << std::endl;
			return;
		}

		if (!SDL_BindAudioStream(device, stream)) {
			std::cerr << "ERROR (Audio): Failed to bind SDL_AudioStream to SDL_AudioDeviceID\n";
			std::cerr << "SDL: " << SDL_GetError() << std::endl;
		}
	}
}