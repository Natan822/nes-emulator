#include "../CPU/CPU.h"
#include <stdint.h>
#include "Controller.h"

Controller::Controller() {}
Controller::~Controller() {}

uint8_t Controller::getInput() {
	return (right << 7 | left << 6 | down << 5 | up << 4 | start << 3 | select << 2 | b << 1 | a);
}

void Controller::buttonPress(char button) {
	switch (button)
	{
	// Up
	case 'U':
		up = true;
		break;

	// Down
	case 'D':
		down = true;
		break;

	// Left
	case 'L':
		left = true;
		break;

	// Right
	case 'R':
		right = true;
		break;

	// Start
	case 'S':
		start = true;
		break;

	// Select
	case 'E':
		select = true;
		break;

	// B
	case 'B':
		b = true;
		break;

	// A
	case 'A':
		a = true;
		break;
	}
}

void Controller::buttonRelease(char button) {
	switch (button)
	{
	// Up
	case 'U':
		up = false;
		break;

	// Down
	case 'D':
		down = false;
		break;

	// Left
	case 'L':
		left = false;
		break;

	// Right
	case 'R':
		right = false;
		break;

	// Start
	case 'S':
		start = false;
		break;

	// Select
	case 'E':
		select = false;
		break;

	// B
	case 'B':
		b = false;
		break;

	// A
	case 'A':
		a = false;
		break;
	}
}