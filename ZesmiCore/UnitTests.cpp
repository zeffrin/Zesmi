
#include "initialize.hpp"
#include "connectioncontroller.hpp"
#include "vector.hpp"


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <list>

using namespace std;

int testVectorAssignment();
int testVectorAddAssign();
int testInitAndDeinit();
int testConnectionSocketListen();

typedef int (*fn)();

static fn tests[] = {
    // List tests here, if tests depend on some other functionality put them afterward
    //testListenandAccept,
    testVectorAssignment,
    testVectorAddAssign,
    testInitAndDeinit,
    testConnectionSocketListen,
    NULL

};

char *testnames[] = {
    // And here as strings
    "Test = overload for Vector class",
    "Test += overload for Vector class",
    "Test Initialisation and Deinitialisation",
    "Test Opening a listening tcp socket"
};

int main(void)
{
    for(int i = 0; tests[i] != NULL; i++)
    {
        printf("%s%*s:   ", testnames[i], 69 - strlen(testnames[i]), "");
        if(tests[i]() == 0)
        {
            printf("PASSED\n");
        }
        else
        {
            printf("FAILED\n");
            break;
        }
    }
    int t;
    fscanf(stdin, "%c", &t);
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

// TODO test for endListen once can connect()



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
