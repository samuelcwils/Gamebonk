#include "IO.h"

IO::IO(bus* Bus, uint16_t* framebuffer)
{
	this->Bus = Bus;
	SrcBuffer = framebuffer;
}

IO::~IO()
{
}

void IO::createWindow(int input_w, int input_h, int input_gw, int input_gh)
{
	w = input_w;
	h = input_h;

	gw = input_gw;
	gh = input_gh;

	window = SDL_CreateWindow("Gamebonk", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_RESIZABLE);

	renderer = SDL_CreateRenderer(window, -1, 0);

	gameScreen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, gw, gh);

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "SDL INITIALIZATION ERROR: \n" << SDL_GetError();
	}

	if (window == NULL)
	{
		std::cout << "SDL COULD NOT CREATE WINDOW: \n" << SDL_GetError();
	}

	if (renderer == NULL)
	{
		std::cout << "SDL COULD NOT CREATE RENDERER: \n" << SDL_GetError();
	}

	if (gameScreen == NULL)
	{
		std::cout << "SDL COULD NOT CREATE TEXTURE: \n" << SDL_GetError();
	}
}

void IO::keyInput()
{
		if(SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					SDL_DestroyWindow(window);
					SDL_DestroyRenderer(renderer);
					SDL_DestroyTexture(gameScreen);
					SDL_Quit();
					exit(0);
					break;

				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
						{
							case SDLK_ESCAPE:
								SDL_DestroyWindow(window);
								SDL_DestroyRenderer(renderer);
								SDL_DestroyTexture(gameScreen);
								SDL_Quit();
								exit(0);
								break;

							case SDLK_LSHIFT: //start
								Bus->interruptFlags(0b00010000);
								Bus->joypad_state &= 0b01111111;
								break;
							
							case SDLK_LCTRL: //select
								Bus->interruptFlags(0b00010000);
								Bus->joypad_state &= 0b10111111;
								break;
							
							case SDLK_z: //a
								Bus->interruptFlags(0b00010000);
								Bus->joypad_state &= 0b11101111;
								break;

							case SDLK_x: //b
								Bus->interruptFlags(0b00010000);
								Bus->joypad_state &= 0b11011111;
								break;

							case SDLK_UP:
								Bus->interruptFlags(0b00010000);
								Bus->joypad_state &= 0b11111011;
								break;
							
							case SDLK_DOWN:
								Bus->interruptFlags(0b00010000);
								Bus->joypad_state &= 0b11110111;
								break;
							
							case SDLK_LEFT:
								Bus->interruptFlags(0b00010000);
								Bus->joypad_state &= 0b11111101;
								break;

							case SDLK_RIGHT:
								Bus->interruptFlags(0b00010000);
								Bus->joypad_state &= 0b11111110;
								break;
						}
					break;
				
				case SDL_KEYUP:
					switch (event.key.keysym.sym)
						{
							case SDLK_ESCAPE:
								SDL_DestroyWindow(window);
								SDL_DestroyRenderer(renderer);
								SDL_DestroyTexture(gameScreen);
								SDL_Quit();
								exit(0);
								break;

							case SDLK_LSHIFT: //start
								Bus->joypad_state |= 0b10000000;
								break;
							
							case SDLK_LCTRL: //select
								Bus->joypad_state |= 0b01000000;
								break;
							
							case SDLK_z: //a
								Bus->joypad_state |= 0b00010000;
								break;

							case SDLK_x: //b
								Bus->joypad_state |= 0b00100000;
								break;

							case SDLK_UP:
								Bus->joypad_state |= 0b00000100;
								break;
							
							case SDLK_DOWN:
								Bus->joypad_state |= 0b00001000;
								break;
							
							case SDLK_LEFT:
								Bus->joypad_state |= 0b00000010;
								break;

							case SDLK_RIGHT:
								Bus->joypad_state |= 0b00000001;
								break;
						}
					break;

			}
		}

}


void IO::updateDisplay()
{
	SDL_UpdateTexture(gameScreen, NULL, SrcBuffer, gw * (sizeof(uint16_t)));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, gameScreen, NULL, NULL);
	SDL_RenderPresent(renderer);

}
