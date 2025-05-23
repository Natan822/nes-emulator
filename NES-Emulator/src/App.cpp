#include "App.h"
#include "NES/NES.h"
#include <memory>
#include <thread>

class NES;
namespace App
{
	std::chrono::high_resolution_clock::time_point lastFrameTime = std::chrono::high_resolution_clock::now();;
	std::unique_ptr<NES> nes;

	void syncFrame()
	{
		auto delta = std::chrono::duration<float, std::chrono::microseconds::period>
			(std::chrono::high_resolution_clock::now() - lastFrameTime).count();
		// 0,016639488834903
		if (delta < 16000)
		{
			std::this_thread::sleep_for(std::chrono::duration<double, std::micro>
				(16000 - delta));
		}
		lastFrameTime = std::chrono::high_resolution_clock::now();
	}

	void run(std::string path)
	{
		int windowScale = 3;
		float speed = 1;
		bool displayPatternTables = false;

		nes = std::make_unique<NES>(windowScale, speed);
		nes->loadROM(path);
		nes->start();
	}
}