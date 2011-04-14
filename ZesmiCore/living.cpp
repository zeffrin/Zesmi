
#include "living.hpp"

Living::Living()
{
    _health = 100;
    _armor = 0;
}

Living::~Living()
{
}

short int Living::getHealth()
{
    return _health;
}

short int Living::getArmor()
{
    return _armor;
}

void Living::takeDamage(int damage)
{
    // TODO better damage calculations
    _armor -= (int)(damage);
    if(_armor < 0)
    {
        _health += (_armor);
        _armor = 0;
    }
}

