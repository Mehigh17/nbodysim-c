#ifndef GENERIC_TYPES_H
#define GENERIC_TYPES_H

#include <SDL2/SDL.h>

// Type Constants

// General Types

// Object Types

typedef struct
{
    float X;
    float Y;
} Vector2;

typedef struct
{
    float X;
    float Y;
    float Z;
} Vector3;

typedef struct
{
    Vector2 position;
    Vector2 velocity;
    float charge;
    float mass;
} Particle;

typedef struct
{
    long computationCount;
    int particleCount;
    Particle *particles;
    float simulationTime;

    float normalizationConstant;
    float timeStep;

    SDL_Rect boundingBox;
} Simulation;

#endif