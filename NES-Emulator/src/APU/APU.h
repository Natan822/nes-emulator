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
		void updateSweep(bool isPulse1); // Update sweep's target period
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

	float cycles{};
	CycleState currentCycleState{};

	void feedAudioBuffer(float data);
	float mixerOutput(); // APU Mixer
	unsigned int m_samplesFed{};
private:
	bool m_updateFrame{};
	//constexpr static float samplesPerFrame = 1789773.0 / 44100.0;
	int samplesPerFrame = 40;

	float samplesSum{};
	float samplesCount{};
	void outputSamples();

	bool enableDMC{};
	bool enableNoise{};
	bool enableTriangle{};
	bool enablePulse1{};
	bool enablePulse2{};

	// Frame Counter flags
	bool sequencerMode{}; // 0 = 4-step sequence; 1 = 5-step sequence
	bool interruptInhibit{};
	bool frameInterrupt{};
	bool shouldResetFrameCounter{};
	int8_t cyclesUntilReset{};
	void resetFrameCounter();

	void quarterFrame();
	void halfFrame();

	Pulse pulse1{};
	Pulse pulse2{};
	float pulseTable[31];

	float tndTable[203];

	void updateStatus(uint8_t data);
};