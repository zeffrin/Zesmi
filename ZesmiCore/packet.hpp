
#ifndef PACKET_HPP_INCLUDED
#define PACKET_HPP_INCLUDED

typedef enum pt {
    DUNNO,
    UPDATEBLOCK,
    UPDATEENT
} PacketType;

class Packet
{
    public:
        Packet();
        ~Packet();

    private:
        PacketType _type;
};

#endif
