#include <game.hh>

#include <SDL.h>
#include <iostream>

static bool gameLoop(uint32_t msSinceLast)
{
	static unsigned int keys = Input::NONE;
	SDL_Event ev;

	while (SDL_PollEvent(&ev))
	{
		if (ev.type == SDL_QUIT)
		{
			return false;
		}

		if (ev.type == SDL_KEYDOWN)
		{
			if (ev.key.keysym.sym == SDLK_UP)
				keys |= Input::UP;
			if (ev.key.keysym.sym == SDLK_LEFT)
				keys |= Input::LEFT;
			if (ev.key.keysym.sym == SDLK_RIGHT)
				keys |= Input::RIGHT;
			if (ev.key.keysym.sym == SDLK_q)
				return false;
		}
		if (ev.type == SDL_KEYUP)
		{
			if (ev.key.keysym.sym == SDLK_UP)
				keys &= ~Input::UP;
			if (ev.key.keysym.sym == SDLK_LEFT)
				keys &= ~Input::LEFT;
			if (ev.key.keysym.sym == SDLK_RIGHT)
				keys &= ~Input::RIGHT;
		}
	}

	handleInput(keys);
	update(msSinceLast);
	display();

	return true;
}

int main(int argc, const char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;

		return 1;
	}

	SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, 1800, 1500,
			SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_SHOWN);
	if (win == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();

		return 1;
	}

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr)
	{
		SDL_DestroyWindow(win);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	init(win, ren);

	uint32_t start = SDL_GetTicks();
	while (1)
	{
		uint32_t now = SDL_GetTicks();
		if (!gameLoop(now - start))
			break;

		start = now;

		SDL_RenderPresent(ren);
		SDL_Delay(20);
	}

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}
