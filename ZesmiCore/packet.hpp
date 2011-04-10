
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
    byte ProtocolVersion;
    char *Username;
    char *VerificationKey;
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
    char *Message;
} PlayerMessage;


typedef struct
{
    byte PacketID;
    byte ProtocolVersion;
    char *ServerName;
    char *ServerMOTD;
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
    byte *Data;
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
    char *PlayerName;
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
    char *Message;
} ServerMessage;

typedef struct
{
    byte PacketID;
    char *DisconnectReason;
} DisconnectPlayer;

typedef struct
{
    byte PacketID;
    byte UserType;
} UpdateUserType;

#endif
