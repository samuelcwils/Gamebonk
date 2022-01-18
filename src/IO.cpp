#include "IO.h"

IO::IO(uint8_t interrupts)
{
	this->interrupts = interrupts;
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

	gameScreen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING, gw, gh);

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

						case SDLK_z:
							//interrupts[4] = 1;
							break;
						
					}

				break;

				case SDL_KEYUP:
					break;

			}
		}

}


void IO::updateDisplay(uint8_t* srcBuffer, int srcWidth)
{
	SDL_UpdateTexture(gameScreen, NULL, srcBuffer, srcWidth * (sizeof(uint8_t)));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, gameScreen, NULL, NULL);
	SDL_RenderPresent(renderer);

}
