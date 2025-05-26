#pragma once
#include <vector>
#include <stdint.h>
#include "../CPU/CPU.h"

class APU {

	struct Divider {
		uint8_t period;
		uint8_t counter;

		bool clock();
	};

	struct Envelope {
		bool loopFlag; // Also Length Counter halt flag in Pulse channel
		bool constantVolume;
		uint8_t volume;

		bool startFlag;
		Divider divider;
		uint8_t decayLevelCounter;

		uint8_t output();

		void clock();
	};

	struct Sweep
	{
		bool enabledFlag;
		uint16_t targetPeriod;
		bool negateFlag;
		uint8_t shiftCount;

		Divider divider;
		bool reloadFlag;
	};

	struct Pulse
	{
		uint8_t duty;
		Envelope envelope;
		Sweep sweep;
		int16_t lengthCounter;

		uint16_t currentTimer;
		uint16_t initialTimer;

		uint8_t sequencerPosition;
		bool sequencerOutput;

		bool isEnabled; // Controlled by Status register

		void clock(bool isPulse1);
		void lengthCounterClock();
		void update(uint16_t address, uint8_t data);

		void sweepClock();
		void updateTargetPeriod(bool isPulse1); // Update sweep's target period
		bool isSweepMuting();
	};

	enum CycleState
	{
		ACTIVE,
		IDLE
	};

	static const uint8_t dutyTable[4];
	static const uint8_t lengthTable[0x20];
	static const uint8_t triangleSequence[32];
	static const uint16_t noisePeriodTable[16];
public:

	APU();
	~APU();

	uint8_t readMemoryApu(uint16_t address);
	uint8_t writeMemoryApu(uint16_t address, uint8_t data);

	void clock(CPU* cpu, bool updateFrame = true);
	void halfStep(CPU* cpu);
	void step(CPU* cpu);

	float m_cycles{};
	CycleState m_currentCycleState{};

	void feedAudioBuffer(float data);
	float mixerOutput(); // APU Mixer
	unsigned int m_samplesFed{};
private:
	bool m_updateFrame{};
	//constexpr static float samplesPerFrame = 1789773.0 / 44100.0;
	int m_samplesPerFrame = 40;

	float m_samplesSum{};
	float m_samplesCount{};
	void outputSamples();

	bool m_enableDMC{};
	bool m_enableNoise{};
	bool m_enableTriangle{};
	bool m_enablePulse1{};
	bool m_enablePulse2{};

	// Frame Counter flags
	bool m_sequencerMode{}; // 0 = 4-step sequence; 1 = 5-step sequence
	bool m_interruptInhibit{};
	bool m_frameInterrupt{};
	bool m_shouldResetFrameCounter{};
	int8_t m_cyclesUntilReset{};
	void resetFrameCounter();

	void quarterFrame();
	void halfFrame();

	Pulse pulse1{};
	Pulse pulse2{};
	float pulseTable[31];

	float tndTable[203];

	void updateStatus(uint8_t data);
};