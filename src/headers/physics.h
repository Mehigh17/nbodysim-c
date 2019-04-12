#ifndef PHYSICS_H
#define PHYSICS_H

#include "GenericTypes.h"

#define UNITS_PER_PIXEL (pow(10.0, -3.0))
#define PARTICLE_SIZE 20

void update_physics(Simulation *simulation);

#endif