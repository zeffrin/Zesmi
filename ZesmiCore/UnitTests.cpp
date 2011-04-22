
#include "initialize.hpp"
#include "connectioncontroller.hpp"
#include "vector.hpp"

#include "hrtime.h"


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <list>

using namespace std;

int testHighResolutionTimer();
int testVectorAssignment();
int testVectorAddAssign();
int testInitialize();
int testConnectionSocketListen();
int testSelect();
int testConnectionSocketConnect(); bool testHandler(Packet *p, Connection *sender);
int testConnectionSocketendListen();
int testSendAndReceive();
int testRoutePackets();
int testRespondFromHandler(); bool testRespondHandler(Packet *p, Connection *sender);
int testAllPackets(); bool testAllHandler(Packet *p, Connection *sender);
int testDeinitialize();


typedef int (*fn)();

static fn tests[] = {
    // List tests here, if tests depend on some other functionality put them afterward
    testHighResolutionTimer,
    testVectorAssignment,
    testVectorAddAssign,
    testInitialize,
    testConnectionSocketListen,
    testSelect,
    testConnectionSocketConnect,
    testConnectionSocketendListen,
    testSendAndReceive,
    testRoutePackets,
    testRespondFromHandler,
    testAllPackets,
    testDeinitialize,
    NULL

};

char *testnames[] = {
    // And here as strings
    "Test high resolution timer used for unittesting",
    "Test = overload for Vector class",
    "Test += overload for Vector class",
    "Test Initialization",
    "Test Opening a listening tcp socket",
    "Test select doesn't error with listen sockets",
    "Test connecting to a socket",
    "Test closing a listening socket",
    "Testing send and receive of packets",
    "Test routing packets to a basic handler",
    "Test responding from a handler",
    "Test all packets",
    "Test Deinitialization"
};

int main(void)
{
    stopWatch s;
    double elapsedTime = 0.0f;
    double teT = 0.0f;
    char c;

    if( (sizeof(testnames) / sizeof(char*))  !=  (sizeof(tests) / sizeof(fn)) - 1 )
    {
        printf("Ensure matching entries in tests and testnames in UnitTests.cpp\n");
        printf("Press enter to finish\n");
        fscanf(stdin, "%c", &c);
        return 1;
    }

    int i, passed = 0;
    for(i = 0; tests[i] != NULL; i++)
    {
        printf("%s\n", testnames[i]);
        startTimer(&s);
        int error = tests[i]();
        stopTimer(&s);
        teT = getElapsedTime(&s);
        elapsedTime += teT;

        if(!error)
        {
            printf("Passed in %f seconds.\n\n", teT);
            passed++;
        }
        else
        {
            printf("FAILED in %f seconds.\n\n", teT);
        }
    }

    printf ("%d of %d tests completed successfully in %f seconds.\n", passed, i, elapsedTime);
    printf("Press enter to finish\n");
    fscanf(stdin, "%c", &c);
}

int testHighResolutionTimer()
{
    float a;
    for(long int i = 0; i < 9999999; i++) a = 5 / 7;
    return 0;
}

int testVectorAssignment()
{
    Vector a;
    Vector b;

    a.X = 1.0f;
    a.Y = 2.0f;
    a.Z = 3.0f;
    a.Pitch = 3.0f;
    a.Yaw = 3.0f;

    b = a;

    if (b.X != 1.0f || b.Y != 2.0f || b.Z != 3.0f || b.Pitch != 3.0f || b.Yaw != 3.0f)
    {
        return 1;
    }
    return 0;
}

int testVectorAddAssign()
{
    Vector a;
    Vector b;

    a.X = 1.0f;
    a.Y = 1.0f;
    a.Z = 1.0f;
    a.Pitch = 1.0f;
    a.Yaw = 1.0f;

    b = a;
    b += a;

    if(b.X != 2.0f || b.Y != 2.0f || b.Z != 2.0f || b.Pitch != 2.0f || b.Yaw != 2.0f)
    {
        return 1;
    }
    return 0;
}

int testInitialize()
{
    Initialize *init = Initialize::getInstance();
    bool result = init->doInitialization();
    if(!result)
        return 1;
    else
        return 0;
}

bool testHandler(Packet *p, Connection *sender)
{
    if(!p || !sender)
        return false;
    if(p->PacketID < 0)
    {
        return false;
    }
    if(p->PacketID != P_KEEPALIVE && p->PacketID != P_HANDSHAKE)
        return false;

    if(p->PacketID == P_HANDSHAKE && strcmp(((HandShakePacket*)p)->Username, "Zeffrin") != 0 )
        return false;

    return true;
}

int testConnectionSocketListen()
{
    ConnectionController *conns = ConnectionController::getInstance();
    Connection *PlayerListener;
    PacketHandler handler = testHandler;
    bool result;

    if((PlayerListener = conns->doListen("1022", handler)))
        result = true;
    else
        result = false;

    conns->endListen(PlayerListener);

    if(!result)
        return 1;
    else
        return 0;

}

int testSelect()
{
    ConnectionController *conns = ConnectionController::getInstance();
    Connection *PlayerListener;
    PacketHandler handler = testHandler;
    if(!(PlayerListener = conns->doListen("1022", handler)))
        return 1;

    /* Blocks when working so...
    if(!(conns->doSelect()))
    {
        return 1;
    }
    */
    conns->endListen(PlayerListener);
    return 0;

}

int testConnectionSocketConnect()
{
    Initialize *init = Initialize::getInstance();
    ConnectionController *conns = ConnectionController::getInstance();
        PacketHandler handler = testHandler;
    Connection *PlayerListener;
    Connection *c;

    if(!(PlayerListener = conns->doListen("1022", handler)))
        return 1;

    if(!(c = conns->doConnect("localhost", 1022, handler)))
        return 1;

    conns->doSelect(); // must do select before can do anything else
    while(conns->doAccept() == 0) conns->doSelect();

    conns->endListen(PlayerListener);
    conns->doDisconnect(c);

    return 0;

}

int testConnectionSocketendListen()
{
    Initialize *init = Initialize::getInstance();
    ConnectionController *conns = ConnectionController::getInstance();
    PacketHandler handler = testHandler;
    Connection *PlayerListener;
    Connection *c;

    if(!(PlayerListener = conns->doListen("1022", handler)))
        return 1;

    if(!(c = conns->doConnect("localhost", 1022, handler)))
        return 1;

    while(!(conns->doSelect()));  // must do select before can do anything else

    conns->endListen(PlayerListener);
    conns->doDisconnect(c);

    //TODO actually test this socket is closed

    return 0;

}

int testSendAndReceive()
{
    ConnectionController *conns = ConnectionController::getInstance();
    PacketHandler handler = testHandler;
    Connection *PlayerListener;
    Connection *c;

    if(!(PlayerListener = conns->doListen("1022", handler)))
        return 1;

    if(!(c = conns->doConnect("localhost", 1022, handler)))
        return 1;

    conns->doSelect(); // must do select before can do anything else

    while(conns->doAccept() == 0) conns->doSelect();

    KeepAlivePacket p;
    p.PacketID = P_KEEPALIVE;

    c->SendPacket((Packet*)&p);
    conns->doSelect();
    while(conns->doRecv() < 1) conns->doSelect();


    HandShakePacket h;
    h.PacketID = P_HANDSHAKE;
    strcpy(h.Username, "Zeffrin");

    c->SendPacket((Packet*)&h);
    conns->doSelect();
    while(conns->doRecv() < 1) conns->doSelect();

    conns->endListen(PlayerListener);
    conns->doDisconnect(c);

    return 0;
}

int testRoutePackets()
{
    ConnectionController *conns = ConnectionController::getInstance();
    PacketHandler handler = testHandler;
    Connection *PlayerListener;
    Connection *c;

    if(!(PlayerListener = conns->doListen("1022", handler)))
        return 1;

    if(!(c = conns->doConnect("localhost", 1022, handler)))
        return 1;

    conns->doSelect(); // must do select before can do anything else

    while(conns->doAccept() == 0) conns->doSelect();

    KeepAlivePacket p;
    p.PacketID = P_KEEPALIVE;

    c->SendPacket((Packet*)&p);
    conns->doSelect();
    while(conns->doRecv() < 1) conns->doSelect();


    HandShakePacket h;
    h.PacketID = P_HANDSHAKE;
    strcpy(h.Username, "Zeffrin");

    c->SendPacket((Packet*)&h);
    conns->doSelect();
    while(conns->doRecv() < 1) conns->doSelect();

    if(conns->doRouting() != 2)
        return 1;

    conns->endListen(PlayerListener);
    conns->doDisconnect(c);

    return 0;
}

bool testRespondHandler(Packet *p, Connection *sender)
{
    if(!p || !sender)
        return false;
    if(p->PacketID < 0)
    {
        return false;
    }
    if(p->PacketID != P_KEEPALIVE && p->PacketID != P_HANDSHAKE)
        return false;

    if(p->PacketID == P_HANDSHAKE && strcmp(((HandShakePacket*)p)->Username, "Zeffrin") != 0 )
        return false;

    if(p->PacketID == P_KEEPALIVE)
    {
        HandShakePacket t;
        t.PacketID = P_HANDSHAKE;
        strcpy(t.Username, "Zeffrin");
        sender->SendPacket((Packet*)&t);
    }
    return true;

}

int testRespondFromHandler()
{
    ConnectionController *conns = ConnectionController::getInstance();
    PacketHandler handler = testRespondHandler;
    Connection *PlayerListener;
    Connection *c;

    if(!(PlayerListener = conns->doListen("1022", handler)))
        return 1;

    if(!(c = conns->doConnect("localhost", 1022, handler)))
        return 1;

    conns->doSelect(); // must do select before can do anything else

    while(conns->doAccept() == 0) conns->doSelect();

    KeepAlivePacket p;
    p.PacketID = P_KEEPALIVE;

    c->SendPacket((Packet*)&p);
    conns->doSelect();
    while(conns->doRecv() < 1) { conns->doSelect();}

    if(conns->doRouting() != 1)
    {
        return 1;
    }
    // Handshake has been sent from handler
    while(conns->doRecv() < 1) { conns->doSelect();}

    if(conns->doRouting() != 1)
    {
        return 1;
    }

    conns->endListen(PlayerListener);
    conns->doDisconnect(c);

    return 0;
}

bool testAllHandler(Packet *p, Connection *sender)
{
    if(!p || !sender)
        return false;
    if(p->PacketID < 0 || p->PacketID > PACKETCOUNT)
    {
        return false;
    }

    printf("receiving %d\n", p->PacketID);
    bool error = false;
    switch(p->PacketID)
    {
        case P_KEEPALIVE: break;
        case P_LOGIN:
        {
            LoginPacket *t = (LoginPacket*)p;
            if(!(t->PacketID == P_LOGIN &&
               t->ProtocolVersion == 15 &&
               strcmp(t->Username, "Zeffrin") == 0 &&
               strcmp(t->VerificationKey, "-") == 0 &&
               t->MapSeed == 101 &&
               t->Dimension == 5)) { error = true; }
            break;
        }
        case P_HANDSHAKE:
            if(strcmp(((HandShakePacket*)p)->Username, "Zeffrin") != 0) error = true;
            break;
        case P_CHAT:
            {
                char *bla = ((ChatPacket*)p)->Message;
                if(strcmp(((ChatPacket*)p)->Message, "Test message a-zA-Z0-9") != 0) error = true;
            }
            break;
        case P_UPDATETIME:
            if(((UpdateTimePacket*)p)->Time != 104) error = true;
            break;
        default:
            error = true;
            break;
    }
    return !error;
}

int testAllPackets()
{
    ConnectionController *conns = ConnectionController::getInstance();
    PacketHandler handler = testAllHandler;
    Connection *PlayerListener;
    Connection *c;

    if(!(PlayerListener = conns->doListen("1022", handler)))
        return 1;

    if(!(c = conns->doConnect("localhost", 1022, handler)))
        return 1;

    conns->doSelect(); // must do select before can do anything else

    while(conns->doAccept() == 0) conns->doSelect();

    for(unsigned int i = 0; i < PACKETCOUNT ; i++)
    {
        bool unknown = false;
        switch(i)
        {
            case P_KEEPALIVE:
            {
                KeepAlivePacket p;
                p.PacketID = P_KEEPALIVE;
                c->SendPacket((Packet*)&p);
                break;
            }
            case P_LOGIN:
            {
                LoginPacket p1;
                p1.PacketID = P_LOGIN;
                p1.ProtocolVersion = 15;
                strcpy(p1.Username, "Zeffrin");
                strcpy(p1.VerificationKey, "-");
                p1.MapSeed = 101;
                p1.Dimension = 5;
                c->SendPacket((Packet*)&p1);
                break;
            }
            case P_HANDSHAKE:
            {
                HandShakePacket p2;
                p2.PacketID = P_HANDSHAKE;
                strcpy(p2.Username, "Zeffrin");
                c->SendPacket((Packet*)&p2);
                break;
            }
            case P_CHAT:
            {
                ChatPacket p3;
                p3.PacketID = P_CHAT;
                strcpy(p3.Message, "Test message a-zA-Z0-9");
                c->SendPacket((Packet*)&p3);
                break;
            }
            case P_UPDATETIME:
            {
                UpdateTimePacket p4;
                p4.PacketID = P_UPDATETIME;
                p4.Time = 104;
                c->SendPacket((Packet*)&p4);
                break;
            }
            case P_PLAYERINVENTORY:
            {
                PlayerInventoryPacket p5;
                p5.PacketID = P_PLAYERINVENTORY;
                p5.EntityID = 101;
                p5.Slot = 5;
                p5.ItemID = 101;
                p5.ItemDamage = 10;
                c->SendPacket((Packet*)&p5);
                break;
            }
            case P_SPAWNPOSITION:
            {
                SpawnPositionPacket p6;
                p6.PacketID = P_SPAWNPOSITION;
                p6.X = 101;
                p6.Y = 202;
                p6.Z = 303;
                c->SendPacket((Packet*)&p6);
                break;
            }
            case P_ACTION:
            {
                ActionPacket p7;
                p7.PacketID = P_ACTION;
                p7.PlayerEntity = 101;
                p7.TargetEntity = 202;
                p7.IsLeftClick = 1;
                c->SendPacket((Packet*)&p7);
                break;
            }
            case P_HEALTH:
            {
                HealthPacket p8;
                p8.PacketID = P_HEALTH;
                p8.Health = 88;
                c->SendPacket((Packet*)&p8);
                break;
            }
            case P_RESPAWN:
            {
                RespawnPacket p9;
                p9.PacketID = P_RESPAWN;
                c->SendPacket((Packet*)&p9);
                break;
            }
            case P_FLYING:
            {
                FlyingPacket p10;
                p10.PacketID = P_FLYING;
                p10.Flying = 1;
                c->SendPacket((Packet*)&p10);
                break;
            }
            case P_PLAYERPOSITION:
            {
                PlayerPositionPacket p11;
                p11.PacketID = P_PLAYERPOSITION;
                p11.X = 101.1111f;
                p11.Y = 202.2222f;
                p11.Stance = 111.1111f;
                p11.Z = 303.3333f;
                p11.Flying = 1;
                c->SendPacket((Packet*)&p11);
                break;
            }
            case P_PLAYERLOOK:
            {
                PlayerLookPacket p12;
                p12.PacketID = P_PLAYERLOOK;
                p12.Yaw = 101.1111f;
                p12.Pitch = 202.2222f;
                p12.Flying = 1;
                c->SendPacket((Packet*)&p12);
                break;
            }
            case P_PLAYERLOOKMOVE:
            {
                PlayerLookMovePacket p13;
                p13.PacketID = P_PLAYERLOOKMOVE;
                p13.X = 101.11111f;
                p13.Y = 202.22222f;
                p13.Stance = 303.33333f;
                p13.Z = 404.44444f;
                p13.Yaw = 101.11111f;
                p13.Pitch = 202.2222f;
                c->SendPacket((Packet*)&p13);
                break;
            }
            case P_BLOCKDIG:
            {
                BlockDigPacket p14;
                p14.PacketID = P_BLOCKDIG;
                p14.Status = 1;
                p14.X = 101;
                p14.Y = 202;
                p14.Z = 303;
                p14.Face = 6;
                c->SendPacket((Packet*)&p14);
                break;
            }
            case P_PLACE:
            {
                PlacePacket p15;
                p15.PacketID = P_PLACE;
                p15.X = 101;
                p15.Y = 202;
                p15.Z = 303;
                p15.Direction = 101;
                p15.ItemID = 32;
                p15.StackSize = 64;
                p15.ItemDamage = 10;
                c->SendPacket((Packet*)&p15);
                break;
            }
            case P_BLOCKITEMSWITCH:
            {
                BlockItemSwitchPacket p16;
                p16.PacketID = P_BLOCKITEMSWITCH;
                p16.ID = 101;
                c->SendPacket((Packet*)&p16);
                break;
            }
            case P_SLEEP:
            {
                SleepPacket p17;
                p17.PacketID = P_SLEEP;
                p17.EntityID = 101;
                p17.field_22042_e = 202;
                p17.field_22040_b = 303;
                p17.field_22044_c = 101;
                p17.field_22043_d = 202;
                c->SendPacket((Packet*)&p17);
                break;
            }
            case P_ARMANIMATION:
            {
                ArmAnimationPacket p18;
                p18.PacketID = P_ARMANIMATION;
                p18.EntityID = 101;
                p18.Animate = 1;
                c->SendPacket((Packet*)&p18);
                break;
            }
            case P_PACKET19:
            {
                P19Packet p19;
                p19.PacketID = P_PACKET19;
                p19.EntityID = 101;
                p19.State = 1;
                c->SendPacket((Packet*)&p19);
                break;
            }
            case P_NAMEDENTITYSPAWN:
            {
                NamedEntitySpawnPacket p20;
                p20.PacketID = P_NAMEDENTITYSPAWN;
                p20.EntityID = 101;
                strcpy(p20.Name, "Zeffrin");
                p20.X = 101;
                p20.Y = 202;
                p20.Z = 303;
                p20.Rotation = 128;
                p20.Pitch = 156;
                p20.CurrentItem = 22;
                c->SendPacket((Packet*)&p20);
                break;
            }
            case P_PICKUPSPAWN:
            {
                PickupSpawnPacket p21;
                p21.PacketID = P_PICKUPSPAWN;
                p21.EntityID = 101;
                p21.ItemID = 22;
                p21.Count = 2;
                p21.ItemDamage = 5;
                p21.X = 101;
                p21.Y = 202;
                p21.Z = 303;
                p21.Rotation = 255;
                p21.Pitch = 100;
                p21.Roll = 30;
                c->SendPacket((Packet*)&p21);
                break;
            }
            case P_COLLECT:
            {
                CollectPacket p22;
                p22.PacketID = P_COLLECT;
                p22.CollectedEntityID = 101;
                p22.CollectorEntityID = 202;
                c->SendPacket((Packet*)&p22);
                break;
            }
            default:
                unknown = true;
                break;
        }
        if(unknown)
            return 1;

        conns->doSelect();
        while(conns->doRecv() < 1) { conns->doSelect();}
        if(conns->doRouting() != 1)
        {
            return 1;
        }
    }
    conns->endListen(PlayerListener);
    conns->doDisconnect(c);
    return 0;
}

int testDeinitialize()
{
    ConnectionController *conns = ConnectionController::getInstance();
    Initialize *init = Initialize::getInstance();
    delete conns;
    delete init;
    return 0;
}

