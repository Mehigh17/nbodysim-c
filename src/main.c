#define LINUX

#ifdef WINDOW
#define SDL_MAIN_HANDLED
#include <SDL.h>
#endif

#ifdef LINUX
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../headers/GenericTypes.h"

#define METERS_PER_PIXEL 10E-31

float dist(float x1, float x2, float y1, float y2);

int main(int argc, char **argv)
{
	const int windowWidth = 1024;
	const int windowHeight = 768;

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	const int particleSize = 25; // image size in pixels

	// Simulation Info
	int computationCount = 0;
	const float electronMass = 1.0;
	const float protonMass = 1836.0 * electronMass;
	const float elementaryCharge = 1.0;
	const float K = 1.0;
	const float permittivity = 1.006; // Environment's permittivity (1.006 for air, 80.0 for water)
	float simulationTime = 0.0;
	
	// Simulation Data
	srand(time(NULL));
	float timeStep = 0.01;
	float frameStep = 0.016;
	int particleCount = (argc == 2 ? atoi(argv[1]) : 10);
	Particle particles[particleCount];

	// Setting initial position in pixels then working with meters
	for(int i = 0; i < particleCount; i++)
	{
		particles[i].posX = rand() % windowWidth;
		particles[i].posY = rand() % windowHeight;
		particles[i].charge = elementaryCharge * (rand() % 2 == 0 ? 1.0 : -1.0 ); // Elementary charge
		particles[i].mass = 1.0;
	}
	// -

	if(SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0)
	{
		printf("Failed to intialize the SDL library. Error: %s\n", SDL_GetError());
	}
	else
	{
		if(SDL_CreateWindowAndRenderer(windowWidth, windowHeight, SDL_WINDOW_SHOWN, &window, &renderer) == 0)
		{
			int run = 1;

			SDL_Color fontColor = {255, 255, 255};
			int fontSize = 18;
			TTF_Font* font = TTF_OpenFont("fonts/arial.ttf", fontSize);
			if(!font)
			{
				printf("Failed to load the font file. \nError: %s\n", SDL_GetError());
				run = 0;
			}

			char textString[100];
			SDL_Surface* textSurface;
			SDL_Texture* textTexture;
			SDL_Rect destRectText;
			destRectText.x = 10;
			destRectText.y = 10;
			destRectText.w = fontSize;
			destRectText.h = fontSize;

			SDL_Event event;

			SDL_Texture* positiveParticleTexture = IMG_LoadTexture(renderer, "images/positive_particle.png");
			SDL_Texture* negativeParticleTexture = IMG_LoadTexture(renderer, "images/negative_particle.png");
			SDL_Texture* neutralParticleTexture = IMG_LoadTexture(renderer, "images/neutral_particle.png");

			if(positiveParticleTexture == NULL || negativeParticleTexture == NULL || neutralParticleTexture == NULL)
			{
				printf("Error while loading the textures files.\n");
				run = 0;
			}

			while(run)
			{
				simulationTime = SDL_GetTicks() / 1000.0; // seconds

				sprintf(textString, "Particles count: %i, Computation loop count: %i, Simulaton time: %.3fs, using SDL2", particleCount, computationCount, simulationTime);
				destRectText.w = fontSize * strlen(textString) / 2;

				textSurface = TTF_RenderText_Solid(font, textString, fontColor);
				textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

				while(SDL_PollEvent(&event))
					if(event.type == SDL_QUIT || event.type == SDL_KEYDOWN)
						run = 0;

				SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                SDL_RenderCopy(renderer, textTexture, NULL, &destRectText);

				for(int i = 0; i < particleCount; i++)
				{
					SDL_Rect destRect;
					destRect.x = particles[i].posX;
					destRect.y = particles[i].posY;
					destRect.w = particleSize;
					destRect.h = particleSize;

					if(particles[i].charge > 0.0)
					{
						SDL_RenderCopy(renderer, positiveParticleTexture, NULL, &destRect);
					}
					else if(particles[i].charge < 0.0)
					{
						SDL_RenderCopy(renderer, negativeParticleTexture, NULL, &destRect);
					}
					else
					{
						SDL_RenderCopy(renderer, neutralParticleTexture, NULL, &destRect);
					}

					float forceX = 0.0, forceY = 0.0;
					for(int j = 0; j < particleCount; j++)
					{
						if(i != j)
						{
							float distance = dist(particles[i].posX, particles[j].posX, particles[i].posY, particles[j].posY); // In pixels
							if(distance <= (particleSize / 50.0))
							{
								particles[i].charge = particles[j].charge = 0.0;
								particles[j].posX = particles[i].posX;
								particles[j].posY = particles[i].posY;
								break;
							}

							float chargeProduct = particles[i].charge * particles[j].charge;
							float distanceDenominator = pow(distance, 3.0/2.0) * METERS_PER_PIXEL; // In meters

							// Force dimesion: ([M=kg][L=meter])/[T=seconds]^-2
							forceX += (particles[i].posX - particles[j].posX) * (K/permittivity) * chargeProduct / distanceDenominator;
							forceY += (particles[i].posY - particles[j].posY) * (K/permittivity) * chargeProduct / distanceDenominator;

							if(chargeProduct != 0)
							{
								if(chargeProduct < 0)
								{
									SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
								}
								else if(chargeProduct > 0)
								{
									SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
								}
								SDL_RenderDrawLine(renderer, particles[i].posX + particleSize/2.0, particles[i].posY + particleSize/2.0, particles[j].posX + particleSize/2.0, particles[j].posY + particleSize/2.0);
							}
							computationCount++;
						}
					}

					// [L=meter] = ([Force]/[M=kg] = [L=meter]/[T=seconds]^-2) * [T=seconds]
					particles[i].posX += (forceX / particles[i].mass) * timeStep;
					particles[i].posY += (forceY / particles[i].mass) * timeStep;
				}

				SDL_RenderPresent(renderer);
				SDL_Delay(frameStep * 1000);
			}

			TTF_CloseFont(font);
			SDL_FreeSurface(textSurface);
			SDL_DestroyTexture(textTexture);
			SDL_DestroyTexture(positiveParticleTexture);
			SDL_DestroyTexture(negativeParticleTexture);
			SDL_DestroyTexture(neutralParticleTexture);
		}
		else
		{
			printf("Failed to initialize the SDL window/renderer. \nError: %s\n", SDL_GetError());
		}

		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
	return 0;
}

float dist(float x1, float x2, float y1, float y2)
{
	return sqrt(pow(x2 - x1, 2.0) + pow(y2 - y1, 2.0));
}
