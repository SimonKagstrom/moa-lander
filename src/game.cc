#include <game.hh>

#include <stdio.h>
#include <SDL.h>
#include <math.h>

static double gravity = -9.8;
static double accelerationPerSecond = 40;

class Game
{
public:
	void handleInput(unsigned int mask)
	{
		m_acceleration = (mask & Input::UP) ? 1 : 0;

		m_turning = 0;

		if (mask & Input::LEFT)
		{
			m_turning = -1;
		}

		if (mask & Input::RIGHT)
		{
			m_turning = 1;
		}
	}

	void update(unsigned int msSinceLast)
	{
		double secsSinceLast = msSinceLast / 1000.0;

		m_lander.m_angle = (m_lander.m_angle + m_turning * 3) % 360;
		double angleRad = (m_lander.m_angle / 360.0) * 2 * M_PI;

		if (m_acceleration)
		{
			double dx = sin(angleRad);
			double dy = cos(angleRad);

			m_lander.m_velocity.dy += dy * secsSinceLast * accelerationPerSecond;
			m_lander.m_velocity.dx += dx * secsSinceLast * accelerationPerSecond;
		}

		m_lander.m_velocity.dy += secsSinceLast * gravity;
		m_lander.m_position.y += m_lander.m_velocity.dy;
		m_lander.m_position.x += m_lander.m_velocity.dx;
	}

	void display()
	{
		int windowWidth, windowHeight;

		SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);

		SDL_Rect dst;
		dst.x = m_lander.m_position.x;
		dst.y = windowWidth - m_lander.m_position.y;
		dst.h = m_landerSize[0];
		dst.w = m_landerSize[1];

		SDL_RenderCopyEx(m_renderer, m_landerSprite, NULL, &dst, m_lander.m_angle, NULL, SDL_FLIP_NONE);
	}

	void init(SDL_Window *win, SDL_Renderer *ren)
	{
		m_window = win;
		m_renderer = ren;

		SDL_Surface *bmp = SDL_LoadBMP("lander.bmp");

		if (!bmp)
		{
			printf("Can't load lander image\n");
			exit(1);
		}

		m_landerSprite = SDL_CreateTextureFromSurface(m_renderer, bmp);

		m_lander.m_position.x = 100;
		m_lander.m_position.y = 800;
		m_lander.m_angle = 0;
	}

private:
	Lander m_lander;

	int m_acceleration;
	int m_turning;
	SDL_Window *m_window;
	SDL_Renderer *m_renderer;
	SDL_Texture *m_landerSprite;

	unsigned int m_landerSize[2]{100,100};
};


Game game;

void init(SDL_Window *win, SDL_Renderer *ren)
{
	game.init(win, ren);
}

void update(unsigned int timeSinceLast)
{
	game.update(timeSinceLast);
}

void display()
{
	game.display();
}

void handleInput(unsigned int mask)
{
	game.handleInput(mask);
}
