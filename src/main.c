#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "headers/GenericTypes.h"
#include "headers/calculus.h"

#define UNITS_PER_PIXEL (pow(10.0, -3.0))
#define PARTICLE_SIZE 20

void draw(SDL_Renderer *renderer, Simulation *simulation);
void updatePhysics(Simulation *simulation);

static SDL_Texture *positiveParticleTexture;
static SDL_Texture *negativeParticleTexture;
static SDL_Texture *neutralParticleTexture;

int main(int argc, char **argv)
{
	int windowWidth = 1024;
	int windowHeight = 768;

	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	Simulation simulation;
	// Physics & Simulation constants
	const float referenceMass = 9.11 * pow(10.0, -31.0);   // kg (mass of an electron)
	const float referenceLength = 52.9 * pow(10.0, -12.0); // Bohr Radius (52.9 picometers)
	const float referenceTime = 2.4 * pow(10.0, -17.0);	// Electron orbit time in hydrogen (in seconds)
	const float referenceCharge = 1.6 * pow(10.0, -19.0);  // Coulombs
	const float coulombsConstant = 9.0 * pow(10.0, 9.0);   // Coulombs' Law Constant

	simulation.normalizationConstant = coulombsConstant * pow(referenceTime, 2.0) * pow(referenceCharge, 2.0) / (referenceMass * pow(referenceLength, 3.0));
	printf("New constant: %.f\n", simulation.normalizationConstant);

	const float permittivity = 1; // Environment's permittivity (~1 for air, 80.0 for water)

	// Simulation Data
	srand(time(NULL));
	float frameStep = 0.016; // FPS limiter = (1/.016 = 62fps)
	simulation.computationCount = 0;
	simulation.simulationTime = 0;
	simulation.timeStep = frameStep;
	simulation.particleCount = (argc == 2 ? atoi(argv[1]) : 10);
	Particle particles[simulation.particleCount]; // Particles array
	simulation.particles = particles;

	bool isFullscreen = false;

	// Particle initializer
	for (int i = 0; i < simulation.particleCount; i++)
	{
		particles[i].velocity.X = 0;
		particles[i].velocity.Y = 0;
		particles[i].position.X = rand() % windowWidth;
		particles[i].position.Y = rand() % windowHeight;

		particles[i].charge = (rand() % 2 == 0 ? 1.0 : -1.0); // Elementary charge
		particles[i].mass = 1.0;
	}

	// Display initializer
	SDL_DisplayMode currentDiplayMode;
	if (isFullscreen && !SDL_GetCurrentDisplayMode(0, &currentDiplayMode))
	{
		// Success
		windowWidth = currentDiplayMode.w;
		windowHeight = currentDiplayMode.h;
	}
	else
	{
		// Failure
		windowWidth = 1024;
		windowHeight = 768;
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0)
	{
		printf("Failed to intialize the SDL library. Error: %s\n", SDL_GetError());
	}
	else
	{
		if (SDL_CreateWindowAndRenderer(windowWidth, windowHeight, SDL_WINDOW_SHOWN, &window, &renderer) == 0)
		{
			bool isRunning = true;

			SDL_Color fontColor = {255, 255, 255};
			int fontSize = 18;
			TTF_Font *font = TTF_OpenFont("fonts/arial.ttf", fontSize);
			if (!font)
			{
				printf("Failed to load the font file. \nError: %s\n", SDL_GetError());
				isRunning = false;
			}

			if (isFullscreen)
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

			char textString[100];
			SDL_Surface *textSurface;
			SDL_Texture *textTexture;
			SDL_Rect destRectText;
			destRectText.x = 10;
			destRectText.y = 10;
			destRectText.w = fontSize;
			destRectText.h = fontSize;

			SDL_Event event;

			positiveParticleTexture = IMG_LoadTexture(renderer, "images/positive_particle.png");
			negativeParticleTexture = IMG_LoadTexture(renderer, "images/negative_particle.png");
			neutralParticleTexture = IMG_LoadTexture(renderer, "images/neutral_particle.png");

			if (positiveParticleTexture == NULL || negativeParticleTexture == NULL || neutralParticleTexture == NULL)
			{
				printf("Error while loading the textures files.\n");
				isRunning = false;
			}

			while (isRunning)
			{
				simulation.simulationTime += simulation.timeStep;

				sprintf(textString, "Particles count: %i, Computation loop count: %li, Simulaton time: %.3fs", simulation.particleCount, simulation.computationCount, simulation.simulationTime);
				destRectText.w = fontSize * strlen(textString) / 2;

				textSurface = TTF_RenderText_Solid(font, textString, fontColor);
				textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

				while (SDL_PollEvent(&event))
					if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN)
						isRunning = false;

				SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
				SDL_RenderClear(renderer);
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

				updatePhysics(&simulation);

				SDL_RenderCopy(renderer, textTexture, NULL, &destRectText); // Render Text

				draw(renderer, &simulation);

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

void draw(SDL_Renderer *renderer, Simulation *simulation)
{
	static SDL_Rect destRect;
	static SDL_Texture *particleTexture;

	for (int i = 0; i < (*simulation).particleCount; i++)
	{
		destRect.x = (*simulation).particles[i].position.X;
		destRect.y = (*simulation).particles[i].position.Y;
		destRect.w = PARTICLE_SIZE;
		destRect.h = PARTICLE_SIZE;

		if ((*simulation).particles[i].charge > 0.0)
		{
			particleTexture = positiveParticleTexture;
		}
		else if ((*simulation).particles[i].charge < 0.0)
		{
			particleTexture = negativeParticleTexture;
		}
		else
		{
			particleTexture = neutralParticleTexture;
		}
		SDL_RenderCopy(renderer, particleTexture, NULL, &destRect);
	}
}

void updatePhysics(Simulation *simulation)
{
	for (int i = 0; i < (*simulation).particleCount; i++)
	{
		static Vector2 force;
		force.X = 0;
		force.Y = 0;

		for (int j = 0; j < (*simulation).particleCount; j++)
		{
			if (i != j)
			{
				float chargeProduct = (*simulation).particles[i].charge * (*simulation).particles[j].charge;

				float distance = UNITS_PER_PIXEL * dist((*simulation).particles[i].position.X, (*simulation).particles[j].position.X, (*simulation).particles[i].position.Y, (*simulation).particles[j].position.Y); // In pixels
				if (chargeProduct < -0.9 && distance <= 0.01) // They cancel eachother only if they're of opposite polarity
				{
					(*simulation).particles[i].charge = (*simulation).particles[j].charge = 0.0;
					(*simulation).particles[j].position.X = (*simulation).particles[i].position.X;
					(*simulation).particles[j].position.Y = (*simulation).particles[i].position.Y;

					// Skip this iteration since any further calculation would be completely 1pointless
					continue;
				}

				/**
				* Force dimesion: ([M=kg][L=meter])/[T=seconds]^-2
				* F = K * qA * qB / d²
				*/
				force.X += UNITS_PER_PIXEL * ((*simulation).particles[i].position.X - (*simulation).particles[j].position.X) * (*simulation).normalizationConstant * chargeProduct / (distance * distance);
				force.Y += UNITS_PER_PIXEL * ((*simulation).particles[i].position.Y - (*simulation).particles[j].position.Y) * (*simulation).normalizationConstant * chargeProduct / (distance * distance);

				(*simulation).computationCount++;
			}
		}

		(*simulation).particles[i].velocity.X += (force.X / (*simulation).particles[i].mass) * (*simulation).timeStep;
		(*simulation).particles[i].velocity.Y += (force.Y / (*simulation).particles[i].mass) * (*simulation).timeStep;

		// [L=meter] = ([Force]/[M=kg] = [L=meter]/[T=seconds]^-2) * [T=seconds]
		(*simulation).particles[i].position.X += (*simulation).particles[i].velocity.X * (*simulation).timeStep;
		(*simulation).particles[i].position.Y += (*simulation).particles[i].velocity.Y * (*simulation).timeStep;
	}
}