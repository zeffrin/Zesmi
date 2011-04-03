#ifndef BLOCK_HPP_INCLUDED
#define BLOCK_HPP_INCLUDED

typedef enum bt
{
    AIR,
    STONE,
    DIRT,
    GRASS
} BlockType;

class Block
{
    public:
        Block();
        ~Block();

    private:
        BlockType _typeid;
        double x, y, z;

};
#endif // BLOCK_HPP_INCLUDED
