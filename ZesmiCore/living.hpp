#ifndef LIVING_HPP_INCLUDED
#define LIVING_HPP_INCLUDED

#include "vector.hpp"

class Living
{
    public:
        Living();
        ~Living();

        short int getHealth();
        short int getArmor();
        void takeDamage(int damage);
        Vector getVector();
        Position getPosition();


    private:
        short int _health;
        short int _armor;
        Vector _vector;
};
#endif // LIVING_HPP_INCLUDED
