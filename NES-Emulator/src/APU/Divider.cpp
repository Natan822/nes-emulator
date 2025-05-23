#include "APU.h"

bool APU::Divider::clock() {
	if (counter == 0)
	{
		counter = period;
		return true;
	}
	counter--;
	return false;
}