#include "../Audio/Audio.h"
#include "SDL.h"
#include "../CPU/CPU.h"
#include "APU.h"
#include "../App.h"
#include <iostream>

const uint8_t APU::dutyTable[4] =
{
		0b01000000,
		0b01100000,
		0b01111000,
		0b10011111
};

const uint8_t APU::lengthTable[0x20] =
{
	10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14,
	12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
};

const uint8_t APU::triangleSequence[32] =
{
	15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0,
	0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
};

const uint16_t APU::noisePeriodTable[16] =
{
	4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068
};

APU::APU()
{
	pulseTable[0] = 0;
	for (int i = 1; i < 31; i++)
	{
		pulseTable[i] = 95.52 / (8128.0 / i + 100);
	}

	tndTable[0] = 0;
	for (int i = 1; i < 203; i++)
	{
		tndTable[i] = 163.67 / (24329.0 / i + 100);
	}

	m_currentCycleState = CycleState::IDLE;
}

APU::~APU() {}

void APU::clock(CPU* cpu, bool updateFrame)
{
	m_updateFrame = updateFrame;
	if (m_currentCycleState == CycleState::IDLE)
	{
		halfStep(cpu);
	}
	else
	{
		step(cpu);
	}
}

void APU::halfStep(CPU* cpu) {
	m_cycles += 0.5f;
	// Mode 0
	if (m_sequencerMode == 0)
	{
		if (m_cycles == 3728.5 || m_cycles == 7456.5 || m_cycles == 11185.5)
		{
			quarterFrame();
			if (m_cycles == 7456.5)
			{
				halfFrame();
			}
		}
		else if (m_cycles == 14914.5)
		{
			quarterFrame();
			halfFrame();
			if (!m_interruptInhibit)
			{
				m_frameInterrupt = true;
				cpu->irqInterrupt = true;
			}
		}
	}
	// Mode 1
	else
	{
		if (m_cycles == 3728.5 || m_cycles == 7456.5 || m_cycles == 11185.5 || m_cycles == 18640.5)
		{
			quarterFrame();

			if (m_cycles == 7456.5 || m_cycles == 18640.5)
			{
				halfFrame();
			}

		}
	}
	m_currentCycleState = CycleState::ACTIVE;

	pulse1.updateTargetPeriod(true);
	pulse2.updateTargetPeriod(false);

	m_samplesSum += mixerOutput();
	m_samplesCount++;
	if (m_samplesCount >= m_samplesPerFrame) // 40 CPU cycles
	{
		outputSamples();
	}
	
	if (m_shouldResetFrameCounter)
	{
		resetFrameCounter();
	}
}

void APU::step(CPU* cpu) {
	this->m_cycles += 0.5f;

	if (this->m_cycles >= 14914.0 && m_sequencerMode == 0)
	{
		if (!m_interruptInhibit)
		{
			m_frameInterrupt = true;
			cpu->irqInterrupt = true;
		}
	}
	if ((this->m_cycles >= 14915.0 && m_sequencerMode == 0) || (this->m_cycles == 18641.0 && m_sequencerMode == 1))
	{
		this->m_cycles = 1;
	}
	pulse1.clock(true);
	pulse2.clock(false);

	m_samplesSum += mixerOutput();
	m_samplesCount++;
	if (m_samplesCount >= m_samplesPerFrame) // 40 CPU cycles
	{
		outputSamples();
	}
	if (m_shouldResetFrameCounter)
	{
		resetFrameCounter();
	}

	m_currentCycleState = CycleState::IDLE;
}

#ifdef DEBUG_AUDIO
double phase = 0.0;
const double PI = 3.14159265;
const double frequency = 440.0;
#endif // DEBUG_AUDIO

void APU::feedAudioBuffer(float data) {
#ifdef DEBUG_AUDIO
	float sample = static_cast<float>(std::sin(2.0 * PI * phase));
	phase += frequency / Audio::SAMPLE_RATE;
	if (phase >= 1.0)
	{
		phase -= 1.0;
	}

	SDL_QueueAudio(Audio::device, &sample, sizeof(float));
#else
	SDL_QueueAudio(Audio::device, &data, sizeof(float));
#endif // DEBUG_AUDIO

	m_samplesFed++;
	int samplesQueued = SDL_GetQueuedAudioSize(Audio::device) / sizeof(float);
	if (samplesQueued != 0 && samplesQueued % 735 == 0 && m_updateFrame)
	{
		if (samplesQueued < 735 * 1.5)
		{
			for (size_t i = 0; i < ((735 * 2) - samplesQueued) * 41; i++)
			{
				App::nes->clock(false);
			}
		}
		//App::syncFrame();
		m_samplesFed -= 735;
	}
}

void APU::outputSamples()
{
	feedAudioBuffer(m_samplesSum / static_cast<float>(m_samplesCount));
	//feedAudioBuffer(mixerOutput());
	m_samplesCount = 0;
	m_samplesSum = 0;
	m_samplesPerFrame = m_samplesPerFrame == 40 ? 41 : 40;
}

float APU::mixerOutput() {
	int pulse1Out = 0;
	int pulse2Out = 0;
	if (pulse1.sequencerOutput && pulse1.lengthCounter != 0 && !pulse1.isSweepMuting())
	{
		pulse1Out = pulse1.envelope.output();
	}
	if (pulse2.sequencerOutput && pulse2.lengthCounter != 0 && !pulse2.isSweepMuting())
	{
		pulse2Out = pulse2.envelope.output();
	}

	float pulseOut = 0;
	if (pulse1Out + pulse2Out == 0)
	{
		pulseOut = 0;
	}
	else
	{
		pulseOut = 95.88f / ((8128 / (pulse1Out + pulse2Out)) + 100);
	}

	return pulseOut;
}

void APU::quarterFrame() {
	pulse1.envelope.clock();
	pulse2.envelope.clock();
}

void APU::halfFrame() {
	pulse1.lengthCounterClock();
	pulse2.lengthCounterClock();
	pulse1.sweepClock();
	pulse2.sweepClock();
}

void APU::resetFrameCounter() {
	if (m_cyclesUntilReset == 0)
	{
		m_cycles = 0;
		m_shouldResetFrameCounter = false;
	}
	else
	{
		m_cyclesUntilReset--;
	}
}
