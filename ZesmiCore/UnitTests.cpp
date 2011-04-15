
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
int testConnectionSocketConnect();
int testConnectionSocketendListen();
int testDeinitialize();

typedef int (*fn)();

static fn tests[] = {
    // List tests here, if tests depend on some other functionality put them afterward
    //testListenandAccept,
    testHighResolutionTimer,
    testVectorAssignment,
    testVectorAddAssign,
    testInitialize,
    testConnectionSocketListen,
    testSelect,
    testConnectionSocketConnect,
    testConnectionSocketendListen,
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

    // TODO Display times in a nicer format

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

int testConnectionSocketListen()
{
    Initialize *init = Initialize::getInstance();
    ConnectionController *conns = ConnectionController::getInstance();
    Connection *PlayerListener;
    bool result;

    if((PlayerListener = conns->doListen("1022")))
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
    Initialize *init = Initialize::getInstance();
    ConnectionController *conns = ConnectionController::getInstance();
    Connection *PlayerListener;
    if(!(PlayerListener = conns->doListen("1022")))
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
    Connection *PlayerListener;
    Connection *c;

    if(!(PlayerListener = conns->doListen("1022")))
        return 1;

    if(!(c = conns->doConnect("localhost", 1022)))
        return 1;

    conns->doSelect(); // must do select before can do anything else

    KeepAlive p;
    p.PacketID = P_KEEPALIVE;

    while(conns->doAccept() == 0) {
        //c->SendPacket((Packet*)&p);
        conns->doSelect(); }

    conns->endListen(PlayerListener);

    return 0;

}

int testConnectionSocketendListen()
{
    Initialize *init = Initialize::getInstance();
    ConnectionController *conns = ConnectionController::getInstance();
    Connection *PlayerListener;
    Connection *c;

    if(!(PlayerListener = conns->doListen("1022")))
        return 1;

    if(!(c = conns->doConnect("localhost", 1022)))
        return 1;

    conns->doSelect();  // must do select before can do anything else

    if (!(conns->doAccept()))
        return 1;

    conns->endListen(PlayerListener);

    //if((c = conns->doConnect("localhost", 1022)))
    //    return 1;

    //KeepAlive p;
    //p.PacketID = 0x0;
    //c->SendPacket((Packet *)&p);

    //conns->doSelect(); // must do select before can do anything else

    //if ((conns->doAccept()))
    //    return 1;

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


// TODO fix this test to be listen and connect test
/*

int testListenandAccept() {
    printf("testListenandAccept: ");

    Initialize *init = new Initialize();
    bool result = init->doInitialization();
    Logger *log = Logger::getInstance();
    Connection *PlayerListener;

    if(!result)
    {
        // TODO send to logger
        return 1;
    }

    ConnectionController *conns = ConnectionController::getInstance();
    if((PlayerListener = conns->startListen("1022")))
    {
        log->writeToLog("Listening on PORT 1022");
    }

    else
    {
        log->writeToLog("Error creating listening port");
        return 1;
    }

    while(1)
    {
        conns->doSelect();
        conns->doRecv();
        conns->doError();
        conns->doAccept();

        //conns->doRouting();
        //conns->doSend();
    }

    conns->stopListen(PlayerListener);
    delete PlayerListener;


    delete log;
    delete conns;
    delete init;

    return 0;

}
*/
