#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>

#define PARTICLE_COUNT 2

float dist(float x1, float x2, float y1, float y2);

int main()
{
	const int windowWidth = 500;
	const int windowHeight = 500;

	SDL_Window* window = NULL;
	SDL_Surface* surface = NULL;
	SDL_Surface* particleSurface = NULL;
	int particleSize = 50;

	// Simulation Data
	float posX[PARTICLE_COUNT] = { 50.0, 450.0 };
	float posY[PARTICLE_COUNT] = { 50.0, 450.0 };
//	float charge[PARTICLE_COUNT] = { 1.6 * pow(10.0, -19.0), -1.6 * pow(10.0, -19.0) };
	float charge[PARTICLE_COUNT] = { 1.0, -30.0 };
	float mass[PARTICLE_COUNT] = { 1.0, 1.0 };
	// -

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Failed to intialize the SDL library. Error: %s\n", SDL_GetError());
	}
	else
	{
		window = SDL_CreateWindow ("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
		if(window == NULL)
		{
			printf("Failed to initialize the SDL window. Error: %s\n", SDL_GetError());
		}
		else
		{
			surface = SDL_GetWindowSurface(window);
			SDL_Event event;
			int run = 1;

			particleSurface = SDL_LoadBMP("images/circle.bmp");
			if(particleSurface == NULL)
			{
				printf("Failed to load the particle file. \nError: %s\n", SDL_GetError());
				run = 0;
			}

			float timeStep = 0.001;
			while(run)
			{
				SDL_FillRect(surface, NULL, 0x0);
				while(SDL_PollEvent(&event))
				{
					if(event.type == SDL_QUIT || event.type == SDL_KEYDOWN)
						run = 0;
				}

				for(int i = 0; i < PARTICLE_COUNT; i++)
				{
					//float K = 9.0 * pow(10.0, 9.0);
					float K = 90.0;
					int cPi = (i == 0 ? 1 : 0); // counter particle index

					float distance = dist(posX[i], posX[cPi], posY[i], posY[cPi]);
					float forceX = (posX[i] - posX[cPi]) * K * charge[i] * charge[cPi] / pow(distance, 3.0/2.0);
					float forceY = (posY[i] - posY[cPi]) * K * charge[i] * charge[cPi] / pow(distance, 3.0/2.0);

					printf("Force X (%d) = %f\n", i, forceX);

					posX[i] += (forceX / mass[i]) * timeStep;
					posY[i] += (forceY / mass[i]) * timeStep;

					SDL_Rect destRect;
					destRect.x = posX[i];
					destRect.y = posY[i];
					destRect.y += (forceY / mass[i]) * timeStep;
					SDL_BlitSurface(particleSurface, NULL, surface, &destRect);
				}

				SDL_UpdateWindowSurface(window);
				SDL_Delay(timeStep * 1000);
			}
		}
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
	return 0;
}

float dist(float x1, float x2, float y1, float y2)
{
	return sqrt(pow(x2 - x1, 2.0) + pow(y2 - y1, 2.0));
}
