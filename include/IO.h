#pragma once
#include "SDL2/SDL.h"
#include <array>
#include <iostream>
#include "cpu.h"
#include "bus.h"

class bus;
class IO
{
	private:
		int w; //window width
		int h; //window height

		int gw; //game screen width
		int gh; //game screen

		bus* Bus;

		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Texture* gameScreen;
		SDL_Event event;

		uint16_t* SrcBuffer;

	public:
		IO(bus* Bus, uint16_t* framebuffer);
		~IO();

		void createWindow(int input_w, int input_h, int input_gw, int input_gh);

		uint8_t* convertBuffer(uint8_t* srcBuffer);

		void updateDisplay();

		void keyInput();

		void playSound();
};