#pragma once
#include "NES/NES.h"
#include <chrono>
#include <memory>

class NES;
namespace App
{
	extern std::chrono::high_resolution_clock::time_point lastFrameTime;
	void syncFrame();

	extern std::unique_ptr<NES> nes;
	void run(std::string path);

}