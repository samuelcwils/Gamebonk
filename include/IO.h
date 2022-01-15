#pragma once
#include "SDL2/SDL.h"
#include <iostream>

class chip8_IO
{
	private:
		int w; //window width
		int h; //window height

		int gw; //game screen width
		int gh; //game screen

		struct {
			bool IME;
			std::array <bool, 5> IE;
			std::array <bool, 5> IF;
		} interrupts;

		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Texture* gameScreen;
		SDL_Event event;

	public:
		SDL_IO();
		~SDL_IO();

		void createWindow(int input_w, int input_h, int input_gw, int input_gh);

		void updateDisplay(uint32_t* srcBuffer, int srcWidth);

		void keyInput(bool* controller);

		void playSound();
};