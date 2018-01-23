#ifdef _WIN32
#define SDL_MAIN_HANDLED
#include <SDL.h>
#endif

#ifdef linux
#include <SDL2/SDL.h>
#endif

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define METERS_PER_PIXEL 10E29

float dist(float x1, float x2, float y1, float y2);

int main(int argc, char **argv)
{
	const int windowWidth = 1024;
	const int windowHeight = 768;

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Surface* positiveParticleSurface = NULL;
	SDL_Surface* negativeParticleSurface = NULL;
	SDL_Surface* neutralParticleSurface = NULL;
	int particleSize = 25; // image size in pixels

	// Simulation Data
	srand(time(NULL));
	int particleCount = (argc == 2 ? atoi(argv[1]) : 10);
	float posX[particleCount];
	float posY[particleCount];
	float charge[particleCount];
	float mass[particleCount];
	float K = 8.99 * pow(10.0, 9.0); // Coulomb's Constant
	for(int i = 0; i < particleCount; i++)
	{
		posX[i] = rand() % windowWidth;
		posY[i] = rand() % windowHeight;
		charge[i] = 1.602 * pow(10.0, -19.0) * (rand() % 2 == 0 ? 1.0 : -1.0 ); // Elementary charge
		mass[i] = 1.0;
	}
	// -

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Failed to intialize the SDL library. Error: %s\n", SDL_GetError());
	}
	else
	{
		//window = SDL_CreateWindow("nbodysim-c", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
		if(SDL_CreateWindowAndRenderer(windowWidth, windowHeight, SDL_WINDOW_SHOWN, &window, &renderer) == 0)
		{
			SDL_Event event;
			int run = 1;

			SDL_Rect sourceRect;
			sourceRect.x = 0;
			sourceRect.y = 0;
			sourceRect.w = particleSize;
			sourceRect.y = particleSize;

			SDL_Texture* positiveParticleTexture = IMG_LoadTexture(renderer, "images/positive_particle.png");
			SDL_Texture* negativeParticleTexture = IMG_LoadTexture(renderer, "images/negative_particle.png");
			SDL_Texture* neutralParticleTexture = IMG_LoadTexture(renderer, "images/neutral_particle.png");

			if(positiveParticleTexture == NULL || negativeParticleTexture == NULL || neutralParticleTexture == NULL)
			{
				printf("Error while loading the textures files.\n");
				run = 0;
			}

			float timeStep = 0.001;
			while(run)
			{
				while(SDL_PollEvent(&event))
					if(event.type == SDL_QUIT || event.type == SDL_KEYDOWN)
						run = 0;

				SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

				for(int i = 0; i < particleCount; i++)
				{
					SDL_Rect destRect;
					destRect.x = posX[i];
					destRect.y = posY[i];
					destRect.w = particleSize;
					destRect.h = particleSize;

					if(charge[i] > 0.0)
					{
						SDL_RenderCopy(renderer, positiveParticleTexture, NULL, &destRect);
					}
					else if(charge[i] < 0.0)
					{
						SDL_RenderCopy(renderer, negativeParticleTexture, NULL, &destRect);
					}
					else
					{
						SDL_RenderCopy(renderer, neutralParticleTexture, NULL, &destRect);
					}

					for(int j = 0; j < particleCount; j++)
					{
						if(i != j)
						{
							float distance = dist(posX[i], posX[j], posY[i], posY[j]);
							if(distance <= (particleSize / 2.0))
							{
								charge[i] = charge[j] = 0.0;
								posX[j] = posX[i];
								posY[j] = posY[i];
								break;
							}

							float forceX = (posX[i] - posX[j]) * K * charge[i] * charge[j] / pow(distance, 3.0/2.0);
							float forceY = (posY[i] - posY[j]) * K * charge[i] * charge[j] / pow(distance, 3.0/2.0);

							// speed = acceleration * time
							posX[i] += (forceX / mass[i]) * timeStep * METERS_PER_PIXEL;
							posY[i] += (forceY / mass[i]) * timeStep * METERS_PER_PIXEL;

							SDL_RenderDrawLine(renderer, posX[i] + particleSize/2.0, posY[i] + particleSize/2.0, posX[j] + particleSize/2.0, posY[j] + particleSize/2.0);
						}
					}
				}

				SDL_RenderPresent(renderer);
				SDL_Delay(timeStep * 1000);
			}
		}
		else
		{
			printf("Failed to initialize the SDL window/renderer. \nError: %s\n", SDL_GetError());
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