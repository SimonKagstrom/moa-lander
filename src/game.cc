#include <game.hh>

#include <vector>

#include <stdio.h>
#include <SDL.h>
#include <math.h>

static double gravity = -9.8;
static double accelerationPerSecond = 40;

class LandingPad
{
public:
	LandingPad(Point where) :
		m_position(where)
	{
	}

	Point m_position;
};

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

		updateLander(secsSinceLast);
	}

	void display()
	{
		int windowWidth, windowHeight;

		SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);

		SDL_Rect dst;
		dst.x = m_lander.m_position.x;
		dst.y = windowWidth - m_lander.m_position.y;
		dst.h = m_landerSize[1];
		dst.w = m_landerSize[0];

		SDL_SetRenderDrawColor(m_renderer, 0,0,0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(m_renderer);

		drawStars();
		drawLandscape();

		SDL_RenderCopyEx(m_renderer, m_landerSprite, NULL, &dst, m_lander.m_angle, NULL, SDL_FLIP_NONE);
	}

	void init(SDL_Window *win, SDL_Renderer *ren)
	{
		m_window = win;
		m_renderer = ren;
		int windowWidth, windowHeight;

		SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);

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

		// Stars
		for (unsigned i = 0; i < 20; i++)
		{
			Point star;

			star.x = rand() % windowWidth;
			star.y = rand() % windowHeight;

			m_stars.push_back(star);
		}

		generateLandscape(windowWidth, windowHeight, 0, windowWidth);
	}

private:
	void generateLandscape(int windowWidth, int windowHeight, unsigned int startX, unsigned int endX)
	{
		unsigned int y = windowHeight * 0.75;
		unsigned int x = startX;
		unsigned int slopeHeight = 160;

		unsigned int nLines = (endX - startX) / 20;

		for (auto i = 0; i < nLines; i++)
		{
			Line line;

			line.begin.x = x;
			line.begin.y = y;

			y = y + slopeHeight / 2 - rand() % slopeHeight;

			line.end.x = line.begin.x + nLines;
			line.end.y = y;

			m_landscape.push_back(line);

			x += nLines;
		}
	}

	void drawStars()
	{
		SDL_SetRenderDrawColor(m_renderer, 255,255,255, SDL_ALPHA_OPAQUE);

		for (auto &star : m_stars)
		{
			SDL_RenderDrawPoint(m_renderer, star.x, star.y);
		}
	}

	void drawLandscape()
	{
		SDL_SetRenderDrawColor(m_renderer, 0,255,0, SDL_ALPHA_OPAQUE);

		for (auto &line : m_landscape)
		{
			SDL_RenderDrawLine(m_renderer, line.begin.x, line.begin.y, line.end.x, line.end.y);
		}
	}

	void updateLander(double secsSinceLast)
	{
		m_lander.m_angle = (m_lander.m_angle + m_turning * 3) % 360;

		double angleRad = (m_lander.m_angle / 360.0) * 2 * M_PI;

		if (m_acceleration)
		{
			double dx = sin(angleRad);
			double dy = cos(angleRad);

			m_lander.m_velocity.dy += dy * secsSinceLast * accelerationPerSecond;
			m_lander.m_velocity.dx += dx * secsSinceLast * accelerationPerSecond;
		}

		addGravity(secsSinceLast, m_lander.m_position, m_lander.m_velocity);
	}


	void addGravity(double secsSinceLast, Point &pos, Vector &velocity)
	{
		velocity.dy += secsSinceLast * gravity;
		pos.y += velocity.dy;
		pos.x += velocity.dx;

	}

	Lander m_lander;

	int m_acceleration;
	int m_turning;
	SDL_Window *m_window;
	SDL_Renderer *m_renderer;
	SDL_Texture *m_landerSprite;
	std::vector<struct Line> m_landscape;

	std::vector<struct Point> m_stars;


	unsigned int m_landerSize[2]{91,299};
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
