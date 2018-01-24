#ifdef _WIN32
#define SDL_MAIN_HANDLED
#include <SDL.h>
#endif

#ifdef linux
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define METERS_PER_PIXEL 10E-31

float dist(float x1, float x2, float y1, float y2);

int main(int argc, char **argv)
{
	const int windowWidth = 1024;
	const int windowHeight = 768;

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	int particleSize = 25; // image size in pixels

	// Simulation Info
	int computationCount = 0;
	// Simulation Data
	srand(time(NULL));
	float timeStep = 0.001;
	int particleCount = (argc == 2 ? atoi(argv[1]) : 10);
	float posX[particleCount];
	float posY[particleCount];
	float charge[particleCount];
	float mass[particleCount];
	float K = 8.99 * pow(10.0, 9.0); // Coulomb's Constant

	// Setting initial position in pixels then working with meters
	for(int i = 0; i < particleCount; i++)
	{
		posX[i] = rand() % windowWidth;
		posY[i] = rand() % windowHeight;
		charge[i] = 1.602 * pow(10.0, -19.0) * (rand() % 2 == 0 ? 1.0 : -1.0 ); // Elementary charge
		mass[i] = 1.0;
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
				sprintf(textString, "Particles count: %i, Computation loop count: %i, using SDL2", particleCount, computationCount);
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
							float distance = dist(posX[i], posX[j], posY[i], posY[j]); // In pixels
							if(distance <= (particleSize / 50.0))
							{
								charge[i] = charge[j] = 0.0;
								posX[j] = posX[i];
								posY[j] = posY[i];
								break;
							}

							float chargeProduct = charge[i] * charge[j];
							float distanceDenominator = pow(distance, 3.0/2.0) * METERS_PER_PIXEL;

							// Force dimesion: ([M=kg][L=meter])/[T=seconds]^-2
							float forceX = (posX[i] - posX[j]) * K * chargeProduct / distanceDenominator;
							float forceY = (posY[i] - posY[j]) * K * chargeProduct / distanceDenominator;

							// [L=meter] = ([Force]/[M=kg] = [L=meter]/[T=seconds]^-2) * [T=seconds]
							posX[i] += (forceX / mass[i]) * timeStep;
							posY[i] += (forceY / mass[i]) * timeStep;

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
								SDL_RenderDrawLine(renderer, posX[i] + particleSize/2.0, posY[i] + particleSize/2.0, posX[j] + particleSize/2.0, posY[j] + particleSize/2.0);
							}
							computationCount++;
						}
					}
				}

				SDL_RenderPresent(renderer);
				SDL_Delay(timeStep * 1000);
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
