#ifndef LIVING_HPP_INCLUDED
#define LIVING_HPP_INCLUDED

class Living
{
    public:
        Living();
        ~Living();

    private:
        short int _health;
        short int _armor;
        double _x, _y, _z;
};
#endif // LIVING_HPP_INCLUDED
