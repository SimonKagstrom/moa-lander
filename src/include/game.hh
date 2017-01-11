#include <SDL.h>

struct Point
{
	double x{0};
	double y{0};
};

struct Vector
{
	double dx{0};
	double dy{0};
};

struct Line
{
	struct Point begin;
	struct Point end;
};

class Lander
{
public:
	Point m_position;
	Vector m_velocity;
	int m_angle{0};
};

class Particle
{
public:
	Point m_position;
	Vector m_velocity;
	double m_secsToLive;
};

enum Input
{
	NONE = 0,
	LEFT = 1,
	RIGHT = 2,
	UP = 4
};


void update(unsigned int timeSinceLast);

void handleInput(unsigned int mask);

void display();

void reset();

void init(SDL_Window *win, SDL_Renderer *ren);
