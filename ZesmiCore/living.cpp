
#include "living.hpp"

Living::Living()
{
    _health = 100;
    _armor = 0;
    _vector.X = 0;
    _vector.Y = 0;
    _vector.Z = 0;
    _vector.Pitch = 0;
    _vector.Yaw = 0;
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

Vector Living::getVector()
{
    return _vector;
}

Position Living::getPosition()
{
    return *((Position *)&_vector);
}
