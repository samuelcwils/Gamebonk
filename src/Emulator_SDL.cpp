#include "Emulator_SDL.h"

chip8_IO::chip8_IO()
{
}

chip8_IO::~chip8_IO()
{
}

void chip8_IO::createWindow(int input_w, int input_h, int input_gw, int input_gh)
{
	w = input_w;
	h = input_h;


	window = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_RESIZABLE);

	renderer = SDL_CreateRenderer(window, -1, 0);

	gameScreen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, gw, gh);

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

void chip8_IO::keyInput(bool* controller)
{
		if (SDL_PollEvent(&event))
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
					}

				break;

				case SDL_KEYUP:
					break;

			}
		}

}

void chip8_IO::updateDisplay(uint32_t* srcBuffer, int srcWidth)
{
	SDL_UpdateTexture(gameScreen, NULL, srcBuffer, srcWidth * (sizeof(uint32_t)));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, gameScreen, NULL, NULL);
	SDL_RenderPresent(renderer);

}
