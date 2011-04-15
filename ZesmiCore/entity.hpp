#ifndef ENTITY_HPP_INCLUDED
#define ENTITY_HPP_INCLUDED

#include "vector.hpp"

class Entity
{
    public:
        Entity();
        ~Entity();

        Vector getVector();
        Vector setVector(Vector v);
        Vector moveVector(Vector v);
        Position getPosition();
        Position setPosition(Position p);
        Position movePosition(Position p);



    private:
        Vector _vector;
};

#endif // ENTITY_HPP_INCLUDED
