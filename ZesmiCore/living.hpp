#ifndef LIVING_HPP_INCLUDED
#define LIVING_HPP_INCLUDED

#include "entity.hpp"

class Living : public Entity
{
    public:
        Living();
        ~Living();

        short int getHealth();
        short int getArmor();
        void takeDamage(int damage);

    private:
        short int _health;
        short int _armor;
};
#endif // LIVING_HPP_INCLUDED
