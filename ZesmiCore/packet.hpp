
#ifndef PACKET_HPP_INCLUDED
#define PACKET_HPP_INCLUDED

typedef unsigned char byte;
typedef signed char sbyte;

typedef enum {
    P_KEEPALIVE,
    P_LOGIN,
    P_HANDSHAKE,
    P_CHAT,
    P_UPDATETIME,
    P_PLAYERINVENTORY,
    P_SPAWNPOSITION,
    P_PACKET7,
    P_PACKET8,
    P_PACKET9,
    P_FLYING,
    P_PLAYERPOSITION,
    P_PLAYERLOOK,
    P_PLAYERLOOKMOVE,
    P_BLOCKDIG,
    P_PLACE,
    P_BLOCKITEMSWITCH,
    P_SLEEP,
    P_ARMANIMATION,
    P_PACKET19,
    P_NAMEDENTITYSPAWN,
    P_PICKUPSPAWN,
    P_COLLECT,
    P_VEHICLESPAWN,
    P_MOBSPAWN,
    P_PACKET25,
    P_PACKET26,
    P_PACKET27,
    P_PACKET28,
    P_DESTROYENTITY,
    P_ENTITY,
    P_RELENTITYMOVE,
    P_ENTITYLOOK,
    P_RELENTITYMOVELOOK,
    P_ENTITYTELEPORT,
    P_PACKET38,
    P_PACKET39,
    P_PACKET40,
    P_PRECHUNK,
    P_MAPCHUNK,
    P_MULTIBLOCKCHANGE,
    P_BLOCKCHANGE,
    P_PACKET54,
    P_PACKET60,
    P_PACKET70,
    P_PACKET100,
    P_PACKET101,
    P_PACKET102,
    P_PACKET103,
    P_PACKET104,
    P_PACKET105,
    P_PACKET106      // TODO go up to 255 for kick/dc
} PacketType;


typedef struct
{
    byte PacketID;
} Packet;

typedef struct
{
    byte PacketID;
} KeepAlive;

typedef struct
{
    byte PacketID;
    short ProtocolVersion;
    char Username[64];
    char VerificationKey[64];
    long MapSeed;
    byte Dimension;

} Login;

typedef struct
{
    byte PacketID;
    char Username[64];

} HandShake;

typedef struct
{
    byte PacketID;
    double X;
    double Y;
    double Stance;
    double Z;
    float Yaw;
    float Pitch;
} PlayerLookMove;

#endif
