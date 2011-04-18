
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
    P_ACTION,
    P_HEALTH,
    P_RESPAWN,
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
    P_TITLE,
    P_PACKET26,
    P_PACKET27,
    P_ENTITYMOTION,
    P_DESTROYENTITY,
    P_ENTITY,
    P_ENTITYMOVE,
    P_ENTITYLOOK,
    P_RELENTITYMOVELOOK,
    P_ENTITYTELEPORT,
    P_ENTITYSTATUS,
    P_ENTITYVEHICLE,
    P_PACKET40,
    P_PRECHUNK,
    P_MAPCHUNK,
    P_MULTIBLOCKCHANGE,
    P_BLOCKCHANGE,
    P_SOUNDPACKET, // 54
    P_NONE1,
    P_NONE2,
    P_NONE3,
    P_NONE4,
    P_NONE5,
    P_EXPLOSION,
    P_PACKET70,
    P_PACKET100,
    P_PACKET101,
    P_PACKET102,
    P_PACKET103,
    P_PACKET104,
    P_PACKET105,
    P_PACKET106,
    PACKETCOUNT    // TODO go up to 255 for kick/dc
} PacketType;


typedef struct
{
    byte PacketID;
} Packet;

typedef struct
{
    byte PacketID;
} KeepAlivePacket;

typedef struct
{
    byte PacketID;
    short ProtocolVersion;
    char Username[64];
    char VerificationKey[64];
    long MapSeed;
    byte Dimension;

} LoginPacket;

typedef struct
{
    byte PacketID;
    char Username[64];

} HandShakePacket;

typedef struct
{
    byte PacketID;
    char Message[64];
} ChatPacket;

typedef struct
{
    byte PacketID;
    long Time;
} UpdateTimePacket;

typedef struct
{
    byte PacketID;
    int EntityID;
    short Slot;
    short ItemID;
    short ItemDamage;
} PlayerInventoryPacket;

typedef struct
{
    byte PacketID;
    int X;
    int Y;
    int Z;
} SpawnPositionPacket;

typedef struct
{
    byte PacketID;
    int PlayerEntity;
    int TargetEntity;
    byte IsLeftClick;
} ActionPacket;

typedef struct
{
    byte PacketID;
    short Health;
} HealthPacket;

typedef struct
{
    byte PacketID;
} RespawnPacket;

typedef struct
{
    byte PacketID;
    int Flying;
} FlyingPacket;

typedef struct
{
    byte PacketID;
    double X;
    double Y;
    double Stance;
    double Z;
    int Flying;
} PlayerPositionPacket;

typedef struct
{
    byte PacketID;
    float Yaw;
    float Pitch;
    int Flying;
} PlayerLookPacket;

typedef struct
{
    byte PacketID;
    double X;
    double Y;
    double Stance;
    double Z;
    float Yaw;
    float Pitch;
    int Flying;
} PlayerLookMovePacket;

typedef struct
{
    byte PacketID;
    int Status;
    int X;
    int Y;
    int Z;
    int Face;
} BlockDigPacket;

typedef struct
{
    byte PacketID;
    int X;
    int Y;
    int Z;
    int Direction;
    short ItemID;
    byte StackSize;
    short ItemDamage;
} PlacePacket; // TODO (NOTE) Going to need to read this one carefully looks like

typedef struct
{
    byte PacketID;
    short ID;
} BlockItemSwitchPacket;

typedef struct
{
    byte PacketID;
    int EntityID;
    byte field_22042_e;
    int field_22040_b;
    byte field_22044_c;
    int field_22043_d;
} SleepPacket;

typedef struct
{
    byte PacketID;
    int EntityID;
    byte Animate;
} ArmAnimationPacket;

typedef struct
{
    byte PacketID;
    int EntityID;
    byte State;
} P19Packet;

typedef struct
{
    byte PacketID;
    int EntityID;
    char Name[64];
    int X;
    int Y;
    int Z;
    byte Rotation;
    byte Pitch;
    short CurrentItem;
} NamedEntitySpawnPacket;

typedef struct
{
    byte PacketID;
    int EntityID;
    short ItemID;
    byte Count;
    short ItemDamage;
    int X;
    int Y;
    int Z;
    byte Rotation;
    byte Pitch;
    byte Roll;
} PickupSpawnPacket;

typedef struct
{
    byte PacketID;
    int CollectedEntityID;
    int CollectorEntityID;
} CollectPacket;

typedef struct
{
    byte PacketID;
    int EntityID;
    byte Type;
    int X;
    int Y;
    int Z;
} VehicleSpawnPacket;

typedef struct
{
    byte PacketID;
    int EntityID;
    byte Type;
    int X;
    int Y;
    int Z;
    byte Yaw;
    byte Pitch;
    // TODO not complete, writewatchableobjects()?
} MobSpawnPacket;

typedef struct
{
    byte PacketID;
    char Title[64];
    int X;
    int Y;
    int Z;
    int Direction;
} TitlePacket;

typedef struct
{
    byte PacketID;
} P26Packet;

typedef struct
{
    byte PacketID;
    float field_22035_a;
    float field_22034_b;
    float field_22037_e;
    float field_22036_f;
    bool field_22039_c;
    bool field_22038_d;
} P27Packet;

typedef struct
{
    byte PacketID;
    int EntityID;
    short MotionX;
    short MotionY;
    short MotionZ;
} EntityMotionPacket;

typedef struct
{
    byte PacketID;
    int EntityID;
} DestroyEntityPacket;

typedef struct
{
    byte PacketID;
    int EntityID;
} EntityPacket;

typedef struct
{
    byte PacketID;
    int EntityID;
    byte X;
    byte Y;
    byte Z;
} EntityMovePacket;

typedef struct
{
    byte PacketID;
    int EntityID;
    byte Yaw;
    byte Pitch;
} EntityLookPacket;

typedef struct
{
    byte PacketID;
    int EntityID;
    byte X;
    byte Y;
    byte Z;
    byte Yaw;
    byte Pitch;
} EntityMoveLookPacket;

typedef struct
{
    byte PacketID;
    int EntityID;
    int X;
    int Y;
    int Z;
    byte Yaw;
    byte Pitch;
} EntityTeleportPacket;

typedef struct
{
    byte PacketID;
    int EntityID;
    byte EntityStatus;
} EntityStatusPacket;

typedef struct
{
    byte PacketID;
    int EntityID;
    int VehicleEntityID;
} EntityVehiclePacket;

typedef struct
{
    byte PacketID;
    int EntityID;
    char *field_21018_b; // TODO find out what this is
} P40Packet;

typedef struct
{
    byte PacketID;
    int X;
    int Y;
    bool Mode;
} PreChunkPacket;

typedef struct
{
    byte PacketID;
    int X;
    short Y;
    int Z;
    int XSize;
    int YSize;
    int ZSize;
    int ChunkSize;
    byte *Chunk;
} MapChunkPacket;

typedef struct
{
    byte PacketID;
    int X;
    int Z;
    short Size;
    short *CoordinateArray;
    byte *TypeArray;
    byte *MetaDataArray;
} MultiBlockChangePacket;

typedef struct
{
    byte PacketID;
    int X;
    int Y;
    int Z;
    int Type;
    int MetaData;
} BlockChangePacket;

typedef struct
{
    byte PacketID;
    int X;
    short Y;
    int Z;
    int InstrumentType;
    int Pitch;
} SoundPacket;

typedef struct
{
    byte PacketID;
    double X;
    double Y;
    double Z;
    float Size;
    int DestroyedSize;
    byte l;
    byte li;
    byte j1; // TODO find out what these mean
} ExplosionPacket;

#endif
