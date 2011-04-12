#ifndef VECTOR_HPP_INCLUDED
#define VECTOR_HPP_INCLUDED

typedef struct
{
    double X;
    double Y;
    double Z;
    float Pitch;
    float Yaw;
} Vector;

// Position needs to match head of Vector so you can just cast a vector to
// a position when needed

typedef struct
{
    double X;
    double Y;
    double Z;
} Position;

#endif // VECTOR_HPP_INCLUDED
