

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
    return (_vector = v); // TODO += not exist for Vector..
}

Vector Entity::moveVector(Vector v)
{
    return (_vector += v);
}

Position Entity::getPosition()
{
    return *((Position *)&_vector);
}

Position Entity::setPosition(Position p)
{
    return (*((Position *)&_vector) = p);
}

Position Entity::movePosition(Position p)
{
    return (*((Position *)&_vector) += p); // TODO += not exist for Position...
}
