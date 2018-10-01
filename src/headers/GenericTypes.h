#ifndef GENERIC_TYPES_H
#define GENERIC_TYPES_H

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
    uint64_t computationCount;
    uint32_t particleCount;
    Particle *particles;
    float_t simulationTime;

    float_t normalizationConstant;
    float_t timeStep;

    SDL_Rect boundingBox;
} Simulation;

#endif