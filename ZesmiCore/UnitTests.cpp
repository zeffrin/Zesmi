
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

    int i;
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
        }
        else
        {
            printf("FAILED in %f seconds.\n\n", teT);
            break;
        }
    }

    printf ("\n%d of %d tests completed successfully in %f seconds.\n", i, (int)(sizeof(testnames) / sizeof(char*)), elapsedTime);
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

    conns->doSelect();  // must do select before can do anything else

    if (!(conns->doAccept()))
        return 1;

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

    switch(p->PacketID)
    {
        case P_KEEPALIVE: { return true; }
        case P_LOGIN:
        {
            LoginPacket *t = (LoginPacket*)p;
            if(t->PacketID == P_LOGIN &&
               t->ProtocolVersion == 15 &&
               strcmp(t->)Username, "Zeffrin") == 0 &&
               strcmp(t->VerificationKey, "-") == 0 &&
               t->MapSeed == 101 &&
               t->Dimension == 5)
            {
               return true;
            }
            break;
        }
        case P_HANDSHAKE:
        {
            if(strcmp(((HandShakePacket*)p)->Username, "Zeffrin") == 0)
                return true;
            break;
        }
    }
    return false;
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
                KeepAlivePacket p;
                p.PacketID = P_KEEPALIVE;
                c->SendPacket((Packet*)&p);
                break;
            case P_LOGIN:
                LoginPacket p;
                p.PacketID = P_HANDSHAKE;
                p.ProtocolVersion = 15;
                strcpy(p.Username, "Zeffrin");
                strcpy(p.VerificationKey, "-");
                p.MapSeed = "101";
                p.Dimension = 5;


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

