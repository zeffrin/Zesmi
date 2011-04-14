

#include "entity.hpp"

Entity::Entity()
{
    _vector.X = 0;
    _vector.Y = 0;
    _vector.Z = 0;
    _vector.Pitch = 0;
    _vector.Yaw = 0;
}

Entity::~Entity()
{
}


Vector Entity::getVector()
{
    return _vector;
}

Vector Entity::setVector(Vector v)
{
    return (_vector += v);
}

Vector Entity::moveVector(Vector v)
{
    return (_vector += v);
}

Position Entity::getPosition()
{
    return *((Position *)&_vector);
}

        Vector getVector();

//Position Entity::setPosition(Position p); TODO implement
//Position Entity::movePosition(Position p); TODO implment
