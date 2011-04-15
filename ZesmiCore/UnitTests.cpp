
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
int testInitAndDeinit();
int testConnectionSocketListen();
int testConnectionSocketConnect();
int testConnectionSocketendListen();

typedef int (*fn)();

static fn tests[] = {
    // List tests here, if tests depend on some other functionality put them afterward
    //testListenandAccept,
    testHighResolutionTimer,
    testVectorAssignment,
    testVectorAddAssign,
    testInitAndDeinit,
    testConnectionSocketListen,
    testConnectionSocketConnect,
    testConnectionSocketendListen,
    NULL

};

char *testnames[] = {
    // And here as strings
    "Test high resolution timer used for unittesting",
    "Test = overload for Vector class",
    "Test += overload for Vector class",
    "Test Initialisation and Deinitialisation",
    "Test Opening a listening tcp socket",
    "Test connecting to a socket",
    "Test closing a listening socket"
};

int main(void)
{
    int i;
    stopWatch s;
    double elapsedTime = 0.0f;
    double teT = 0.0f;

    if( (sizeof(testnames) / sizeof(char*))  !=  (sizeof(tests) / sizeof(fn)) - 1 )
    {
        printf("Ensure matching entries in tests and testnames in UnitTests.cpp\n");
        printf("Press enter to finish\n");
        fscanf(stdin, "%d", &i);
        return 1;
    }

    for(i = 0; tests[i] != NULL; i++) // reuse i
    {
        printf("%s\n", testnames[i]);
        startTimer(&s);
        int error = tests[i]();
        stopTimer(&s);
        teT = getElapsedTime(&s);
        elapsedTime += teT;

        if(!error)
        {
            printf("    PASSED in %f seconds.\n\n", teT);
        }
        else
        {
            printf("    FAILED in %f seconds.\n\n", teT);
            break;
        }
    }

    printf ("\n%d of %d tests completed successfully in %f seconds.\n", i, (int)(sizeof(testnames) / sizeof(char*)), elapsedTime);

    printf("Press enter to finish\n");

    fscanf(stdin, "%d", &i);  // reuse i
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

int testInitAndDeinit()
{
    Initialize *init = new Initialize();
    bool result = init->doInitialization();
    delete init;
    if(!result)
        return 1;
    else
        return 0;
}

int testConnectionSocketListen()
{
    Initialize *init = new Initialize();
    bool result = init->doInitialization();
    ConnectionController *conns = ConnectionController::getInstance();
    Connection *PlayerListener;

    if((PlayerListener = conns->doListen("1022")))
        result = true;
    else
        result = false;

    delete conns;
    delete init;

    if(!result)
        return 1;
    else
        return 0;
}

int testConnectionSocketConnect()
{
    Initialize *init = new Initialize();
    bool result = init->doInitialization();
    ConnectionController *conns = ConnectionController::getInstance();
    Connection *PlayerListener;
    Connection *c;

    if(!result)
        return 1;

    if(!(PlayerListener = conns->doListen("1022")))
        return 1;

    if(!(c = conns->doConnect("localhost", "1022")))
        return 1;

    if (!(conns->doAccept()))
        return 1;

    delete conns;
    delete init;

    return 0;

}

int testConnectionSocketendListen()
{
    Initialize *init = new Initialize();
    bool result = init->doInitialization();
    ConnectionController *conns = ConnectionController::getInstance();
    Connection *PlayerListener;
    Connection *c;

    if(!result)
        return 1;

    if(!(PlayerListener = conns->doListen("1022")))
        return 1;

    if(!(c = conns->doConnect("localhost", "1022")))
        return 1;

    if (!(conns->doAccept()))
        return 1;

    if (!conns->endListen(PlayerListener))
        return 1;

    if(!(c = conns->doConnect("localhost", "1022")))
        return 1;

    if ((conns->doAccept()))
        return 1;

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
