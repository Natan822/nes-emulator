#pragma once
#include <chrono>
#include <memory>

class NES;
class Debugger;
namespace App
{
	extern std::chrono::high_resolution_clock::time_point lastFrameTime;
	extern std::unique_ptr<NES> nes;
	extern std::unique_ptr<Debugger> debugger;
	
	void run(std::string path);
	void syncFrame();


}