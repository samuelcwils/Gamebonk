#pragma once
#include "SDL2/SDL.h"
#include <array>
#include <iostream>
#include "cpu.h"

class IO
{
	private:
		int w; //window width
		int h; //window height

		int gw; //game screen width
		int gh; //game screen

		uint8_t interrupts;
		uint8_t joypad;

		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Texture* gameScreen;
		SDL_Event event;

	public:
		IO(uint8_t interrupts);
		~IO();

		void createWindow(int input_w, int input_h, int input_gw, int input_gh);

		uint8_t* convertBuffer(uint8_t* srcBuffer);

		void updateDisplay(uint8_t* srcBuffer, int srcWidth);

		void keyInput();

		void playSound();
};