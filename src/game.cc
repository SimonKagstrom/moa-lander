#include <game.hh>

#include <vector>
#include <deque>

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
		updateParticles(secsSinceLast);
	}

	void display()
	{
		int windowWidth, windowHeight;

		SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);

		SDL_Rect dst;
		dst.x = m_lander.m_position.x;
		dst.y = windowHeight - m_lander.m_position.y;
		dst.h = m_landerSize[1];
		dst.w = m_landerSize[0];

		SDL_SetRenderDrawColor(m_renderer, 0,0,0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(m_renderer);

		drawStars(windowHeight);
		drawLandscape(windowHeight);
		drawLandingPads(windowHeight);
		drawParticles(windowHeight);

		SDL_RenderCopyEx(m_renderer, m_landerSprite, NULL, &dst, m_lander.m_angle, NULL, SDL_FLIP_NONE);
		SDL_SetRenderDrawColor(m_renderer, 255,255,0, SDL_ALPHA_OPAQUE);

		SDL_RenderDrawPoint(m_renderer, dst.x, dst.y);
	}

	void init(SDL_Window *win, SDL_Renderer *ren)
	{
		m_window = win;
		m_renderer = ren;
		int windowWidth, windowHeight;
		int w,h;

		SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);

		SDL_Surface *landerBmp = SDL_LoadBMP("lander.bmp");
		SDL_Surface *sparkBmp = SDL_LoadBMP("sparks.bmp");

		if (!landerBmp || !sparkBmp)
		{
			printf("Can't load lander/sparks image\n");
			exit(1);
		}

		m_landerSprite = SDL_CreateTextureFromSurface(m_renderer, landerBmp);
		m_sparkSprite = SDL_CreateTextureFromSurface(m_renderer, sparkBmp);

		SDL_QueryTexture(m_landerSprite, NULL, NULL, &w, &h);
		m_landerSize[0] = w;
		m_landerSize[1] = h;

		SDL_QueryTexture(m_sparkSprite, NULL, NULL, &w, &h);
		m_sparkSize[0] = w;
		m_sparkSize[1] = h;

		generateLandingPads(windowWidth, windowHeight);
		generateLandscape(windowWidth, windowHeight,
				0, m_pads[0].begin.x, windowHeight * 0.25, m_pads[0].begin.y);
		generateLandscape(windowWidth, windowHeight,
				m_pads[0].end.x, m_pads[1].begin.x, m_pads[0].end.y, m_pads[1].begin.y);
		generateLandscape(windowWidth, windowHeight,
				m_pads[1].end.x, windowWidth, m_pads[0].end.y, windowHeight * 0.25);

		m_lander.m_position = {m_pads[0].begin.x + (m_pads[0].end.x - m_pads[0].begin.x) / 3,
				m_pads[0].begin.y + m_landerSize[1]};
		m_lander.m_angle = 0;

		// Stars
		for (unsigned i = 0; i < 20; i++)
		{
			Point star;

			star.x = rand() % windowWidth;
			star.y = rand() % windowHeight;

			m_stars.push_back(star);
		}

	}

private:
	void generateLandingPads(int windowWidth, int windowHeight)
	{
		Line start, end;

		start.begin.x = windowWidth * 0.10;
		start.begin.y = windowHeight * 0.20;
		start.end.x = start.begin.x + m_landerSize[0] * 2;
		start.end.y = start.begin.y;

		end.begin.x = windowWidth - m_landerSize[0] * 3;
		end.begin.y = windowHeight * 0.20;
		end.end.x = end.begin.x + m_landerSize[0] * 2;
		end.end.y = end.begin.y;

		m_pads.push_back(start);
		m_pads.push_back(end);
	}

	void generateLandscape(int windowWidth, int windowHeight, int startX, int endX,
			int startY, int endY)
	{
		int x = startX;
		int y = startY;
		int slopeHeight = 160;

		unsigned int nLines = abs(endX - startX) / 20;
		unsigned int lineWidth = abs(endX - startX) / nLines;

		for (auto i = 0; i < nLines; i++)
		{
			Line line;

			line.begin.x = x;
			line.begin.y = y;

			y = y + slopeHeight / 2 - rand() % slopeHeight;

			// Last one
			if (i == nLines - 1)
			{
				y = endY;
			}

			line.end.x = line.begin.x + lineWidth;
			line.end.y = y;

			m_landscape.push_back(line);

			x += lineWidth;
		}
	}


	void addParticle(double angle, const Point &where, double maxSpeed)
	{
		if (m_particles.size() > 30)
		{
			return;
		}

		Particle p;

		angle = angle + 20 - rand() % 40;
		maxSpeed = maxSpeed + 2 - rand() % 4;

		double angleRad = (angle / 360.0) * 2 * M_PI;

		p.m_position = where;
		p.m_velocity.dx = sin(angleRad) * maxSpeed;
		p.m_velocity.dy = cos(angleRad) * maxSpeed;
		p.m_secsToLive = 1;

		m_particles.push_back(p);
	}

	void drawStars(unsigned int windowHeight)
	{
		SDL_SetRenderDrawColor(m_renderer, 255,255,255, SDL_ALPHA_OPAQUE);

		for (auto &star : m_stars)
		{
			SDL_RenderDrawPoint(m_renderer, star.x, star.y);
		}
	}

	void drawParticles(unsigned int windowHeight)
	{
		for (auto &particle : m_particles)
		{
			SDL_Rect dst;

			dst.x = particle.m_position.x;
			dst.y = windowHeight - particle.m_position.y;
			dst.w = m_sparkSize[0];
			dst.h = m_sparkSize[1];

			SDL_RenderCopy(m_renderer, m_sparkSprite, NULL, &dst);

			SDL_RenderDrawPoint(m_renderer, particle.m_position.x,
					windowHeight - particle.m_position.y);
		}
	}

	void drawLandscape(unsigned int windowHeight)
	{
		SDL_SetRenderDrawColor(m_renderer, 0,255,0, SDL_ALPHA_OPAQUE);

		for (auto &line : m_landscape)
		{
			SDL_RenderDrawLine(m_renderer, line.begin.x, windowHeight - line.begin.y,
					line.end.x, windowHeight - line.end.y);
		}
	}

	void drawLandingPads(unsigned int windowHeight)
	{
		SDL_SetRenderDrawColor(m_renderer, 0,0,128, SDL_ALPHA_OPAQUE);

		for (auto &line : m_pads)
		{
			SDL_RenderDrawLine(m_renderer, line.begin.x, windowHeight - line.begin.y,
					line.end.x, windowHeight - line.end.y);
		}
	}


	Line *findLineForPoint(const Point &point, std::vector<Line> &lines)
	{
		for (auto &line : lines)
		{
			if (point.x >= line.begin.x && point.x < line.end.x)
			{
				return &line;
			}
		}

		return nullptr;
	}

	void updateParticles(double secsSinceLast)
	{
		bool dequeueFront = false;

		for (auto &particle : m_particles)
		{
			particle.m_secsToLive -= secsSinceLast;
			if (particle.m_secsToLive < 0)
			{
				dequeueFront = true;
			}

			particle.m_velocity.dy += secsSinceLast * gravity;
			particle.m_position.y += particle.m_velocity.dy;
			particle.m_position.x += particle.m_velocity.dx;
		}

		if (dequeueFront)
		{
			m_particles.pop_front();
		}
	}

	void updateLander(double secsSinceLast)
	{
		m_lander.m_angle = (m_lander.m_angle + m_turning * 3) % 360;

		double angleRad = (m_lander.m_angle / 360.0) * 2 * M_PI;

		if (landerIsOnPad())
		{
			if (fabs(m_lander.m_velocity.dy) > fabs(gravity)/2)
			{
				printf("Kaboom! %.3f\n", m_lander.m_velocity.dy);
			}

			m_lander.m_velocity.dx = 0;
			m_lander.m_velocity.dy = 0;
			m_lander.m_angle = 0;
		}
		else
		{
			m_lander.m_velocity.dy += secsSinceLast * gravity;
		}

		if (m_acceleration)
		{
			double dx = sin(angleRad);
			double dy = cos(angleRad);

			m_lander.m_velocity.dy += dy * secsSinceLast * accelerationPerSecond;
			m_lander.m_velocity.dx += dx * secsSinceLast * accelerationPerSecond;

			addThrustFire();
		}

		m_lander.m_position.y += m_lander.m_velocity.dy;
		m_lander.m_position.x += m_lander.m_velocity.dx;
	}

	void addThrustFire()
	{
		Point particlePosition{m_lander.m_position.x, m_lander.m_position.y};
		auto angle = m_lander.m_angle + 180;
		auto angleRad = (angle / 360.0) * 2 * M_PI;

		double dx = sin(angleRad);
		double dy = cos(angleRad);

		particlePosition.x = (m_lander.m_position.x + m_landerSize[0] / 2) + dx * m_landerSize[0];
		particlePosition.y = m_lander.m_position.y + dy * m_landerSize[1];

		addParticle(angle, particlePosition, 4);
	}

	bool landerIsOnPad()
	{
		Point end;

		end.x = m_lander.m_position.x + m_landerSize[0];
		end.y = m_lander.m_position.y;

		Line *first = findLineForPoint(m_lander.m_position, m_pads);
		Line *second = findLineForPoint(end, m_pads);

		if (!first || !second)
		{
			return false;
		}

		if (fabs((m_lander.m_position.y - m_landerSize[1]) - first->begin.y) < 5)
		{
			return true;
		}

		return false;
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
	SDL_Texture *m_sparkSprite;
	std::vector<struct Line> m_landscape;

	std::vector<struct Point> m_stars;
	std::vector<struct Line> m_pads;

	std::deque<Particle> m_particles;

	unsigned int m_landerSize[2];
	unsigned int m_sparkSize[2];
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
