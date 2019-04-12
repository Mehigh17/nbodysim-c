#include "headers/physics.h"
#include "headers/GenericTypes.h"
#include "headers/math.h"

void update_physics(Simulation *simulation)
{
	for (int i = 0; i < simulation->particleCount; i++)
	{
		static Vector2 force;
		force.X = 0;
		force.Y = 0;

		for (int j = 0; j < simulation->particleCount; j++)
		{
			if (i != j)
			{
				float chargeProduct = simulation->particles[i].charge * simulation->particles[j].charge;

				float distance = UNITS_PER_PIXEL * dist(simulation->particles[i].position.X, simulation->particles[j].position.X, simulation->particles[i].position.Y, simulation->particles[j].position.Y); // In pixels
				if (chargeProduct < -0.9 && distance <= 0.01)																																						 // They cancel eachother only if they're of opposite polarity
				{
					simulation->particles[i].charge = simulation->particles[j].charge = 0.0;
					simulation->particles[j].position.X = simulation->particles[i].position.X;
					simulation->particles[j].position.Y = simulation->particles[i].position.Y;

					// Skip this iteration since any further calculation would be completely 1pointless
					continue;
				}

				/**
				* Force dimesion: ([M=kg][L=meter])/[T=seconds]^-2
				* F = K * qA * qB / d²
				*/
				force.X += UNITS_PER_PIXEL * (simulation->particles[i].position.X - simulation->particles[j].position.X) * simulation->normalizationConstant * chargeProduct / (distance * distance);
				force.Y += UNITS_PER_PIXEL * (simulation->particles[i].position.Y - simulation->particles[j].position.Y) * simulation->normalizationConstant * chargeProduct / (distance * distance);

				simulation->computationCount++;
			}
		}

		simulation->particles[i].velocity.X += (force.X / simulation->particles[i].mass) * simulation->timeStep;
		simulation->particles[i].velocity.Y += (force.Y / simulation->particles[i].mass) * simulation->timeStep;

		if (simulation->particles[i].position.X < 0 || simulation->particles[i].position.X > (simulation->boundingBox.w - (PARTICLE_SIZE)))
		{
			simulation->particles[i].velocity.X *= -1.0;
		}

		if (simulation->particles[i].position.Y < 0 || simulation->particles[i].position.Y > (simulation->boundingBox.h - (PARTICLE_SIZE)))
		{
			simulation->particles[i].velocity.Y *= -1.0;
		}

		// [L=meter] = ([Force]/[M=kg] = [L=meter]/[T=seconds]^-2) * [T=seconds]
		simulation->particles[i].position.X += simulation->particles[i].velocity.X * simulation->timeStep;
		simulation->particles[i].position.Y += simulation->particles[i].velocity.Y * simulation->timeStep;
	}
}