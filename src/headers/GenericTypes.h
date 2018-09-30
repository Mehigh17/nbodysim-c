#ifndef GENERIC_TYPES_H
#define GENERIC_TYPES_H

// Type Constants

#define TRUE 1
#define FALSE 0

// General Types

typedef unsigned char BOOL;

// Object Types

typedef struct {
    float X;
    float Y;
} Vector2;

typedef struct {
    float X;
    float Y;
    float Z;
} Vector3;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float charge;
    float mass;
} Particle;

#endif