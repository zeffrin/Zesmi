
#ifndef PACKET_HPP_INCLUDED
#define PACKET_HPP_INCLUDED

typedef unsigned char byte;
typedef signed char sbyte;

typedef enum {
    PLAYERID, //0x00
    PLYRSTBLCK, //0x05
    PLYRVECTOR, // 0x08
    PLYRMSG, // 0x0d
    SERVERID, // 0x00
    PING, // 0x01
    LEVELINIT, // 0x02
    LEVELDATA, // 0x03
    LEVELFINISH, // 0x04
    SRVRSTBLCK, // 0x06
    SPAWNPLAYER, // 0x07
    SRVRVECTOR, // 0x08
    SERVERVECTORUPDATE, // 0x09
    POSITIONUPDATE,  // 0x0a
    ORIENTATIONUPDATE, // 0x0b
    DESPAWNPLAYER, //0x0c
    SERVERMSG, //0x0d
    DISCONNECTPLYR, //0x0e
    UPDATEUSRTYPE //0x0f
} PacketType;

typedef struct
{
    byte PacketID;
} Packet;

typedef struct
{
    byte PacketID;
    byte ProtocolVersion;
    char Username[64];
    char VerificationKey[64];
    byte Unused;

} PlayerID;

typedef struct
{
    byte PacketID;
    short X;
    short Y;
    short Z;
    byte Mode;
    byte BlockType;
} PlayerSetBlock;

typedef struct
{
    byte PacketID;
    byte PlayerID;
    short X;
    short Y;
    short Z;
    byte Yaw;
    byte Pitch;
} PlayerVector;

typedef struct
{
    byte PacketID;
    byte Unused;
    char Message[64];
} PlayerMessage;


typedef struct
{
    byte PacketID;
    byte ProtocolVersion;
    char ServerName[64];
    char ServerMOTD[64];
    byte UserType;
} ServerID;

typedef struct
{
    byte PacketID;
} Ping;

typedef struct
{
    byte PacketID;
} LevelInit;

typedef struct
{
    byte PacketID;
    short Length;
    byte Data[64];
    byte Complete;
} LevelData;

typedef struct
{
    byte PacketID;
    short XSize;
    short YSize;
    short ZSize;
} LevelFinish;

typedef struct
{
    byte PacketID;
    short X;
    short Y;
    short Z;
} ServerSetBlock;

typedef struct
{
    byte PacketID;
    sbyte PlayerID;
    char PlayerName[64];
    short X;
    short Y;
    short Z;
    byte Yaw;
    byte Pitch;
} SpawnPlayer;

typedef struct
{
    byte PacketID;
    sbyte PlayerID;
    short X;
    short Y;
    short Z;
    byte Yaw;
    byte Pitch;
} ServerVector;

typedef struct
{
    byte PacketID;
    sbyte PlayerID;
    short X;
    short Y;
    short Z;
    byte Yaw;
    byte Pitch;

} ServerVectorUpdate;

typedef struct
{
    byte PacketID;
    sbyte PlayerID;
    sbyte ChangeX;
    sbyte ChangeY;
    sbyte ChangeZ;
} PositionUpdate;

typedef struct
{
    byte PacketID;
    sbyte PlayerID;
    byte Yaw;
    byte Pitch;
} OrientationUpdate;

typedef struct
{
    byte PacketID;
    sbyte PlayerID;
} DespawnPlayer;

typedef struct
{
    byte PacketID;
    sbyte PlayerID;
    char Message[64];
} ServerMessage;

typedef struct
{
    byte PacketID;
    char DisconnectReason[64];
} DisconnectPlayer;

typedef struct
{
    byte PacketID;
    byte UserType;
} UpdateUserType;

#endif
