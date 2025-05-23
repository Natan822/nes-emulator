#include "SDL.h"
#include "Audio.h"
#include "../App.h"
#include "../APU/APU.h"
#include <iostream>
#include <vector>
#include <queue>

namespace Audio {
	SDL_AudioDeviceID device;

	std::vector<float> soundBuffer(BUFFER_SIZE);
	std::queue<float> soundQueue;
	int index = 0;
	
	// Unused audio callback
	void audioCallback(void* userdata, Uint8* stream, int len) {
		float* audioStream = reinterpret_cast<float*>(stream);

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

	void initialize() {
		SDL_AudioSpec desiredSpec;

		desiredSpec.freq = SAMPLE_RATE;
		desiredSpec.format = AUDIO_F32LSB;
		desiredSpec.channels = 1;
		desiredSpec.samples = BUFFER_SIZE; 
		desiredSpec.callback = nullptr;
		//desiredSpec.callback = audioCallback;

		device = SDL_OpenAudioDevice(nullptr, 0, &desiredSpec, NULL, 0);
	}
}