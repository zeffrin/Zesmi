#ifndef VECTOR_HPP_INCLUDED
#define VECTOR_HPP_INCLUDED

typedef struct Vector
{
    double X;
    double Y;
    double Z;
    float Pitch;
    float Yaw;
    Vector& operator=(const Vector &rhs);
    Vector& operator+=(const Vector &rhs);
} Vector;

// Position needs to match head of Vector so you can just cast a vector to
// a position when needed

typedef struct Position
{
    double X;
    double Y;
    double Z;
    Position& operator=(const Position &rhs);
    Position& operator+=(const Position &rhs);
} Position;

#endif // VECTOR_HPP_INCLUDED
