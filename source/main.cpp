#pragma once

#include "chip8.hpp"
#include "platform.hpp"
#include <chrono>
#include <iostream>


int main(int argc, char** argv){
	if (argc != 4){
		std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
		std::exit(EXIT_FAILURE);
	}

	int videoScale = std::stoi(argv[1]);
	int cycleDelay = std::stoi(argv[2]);
	char const* romFileName = argv[3];

	Platform platform("Chip8Emulator", videoWidth * videoScale, videoHeight * videoScale, videoWidth, videoHeight);

	Chip8 chip8;
	chip8.loadROM(romFileName);

	int videoPitch = sizeof(chip8.graphics[0]) * videoWidth;

	bool quitToggler = false;
	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	int instructionsPerFrame = 2;

	while (!quitToggler){
		quitToggler = platform.ProcessInput(chip8.keypad);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > cycleDelay){
			lastCycleTime = currentTime;

         for (int i = 0; i < instructionsPerFrame; ++i) chip8.Cycle();

         chip8.updateTimers();

         platform.Update(chip8.graphics, videoPitch);

		}
	}

	return 0;
}