
#include "initialize.hpp"
#include "logger.hpp"
#include "connectioncontroller.hpp"
#include "ANSI.h"

#include <assert.h>
#include <list>

using namespace std;

int testListenandAccept();
int testVectorAssignment();
int testVectorAddition();

typedef int (*fn)();

static fn tests[] = {
    // List tests here, if tests depend on some other functionality put them afterward
    //testListenandAccept,
    testVectorAssignment,
    testVectorAddition,
    NULL

};

char *testnames[] = {
    // And here as strings
    "testVectorAssignment - Tests assignment overload for Vector class",
    "testVectorAddAssign - Tests += overload for Vector class",
};

int main(void)
{
    for(int i = 0; tests[i] != NULL; i++)
    {
        printf("Running %s%*s:   ", testnames[i], 60 - strlen(testnames[i]), " ");
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
    return 0;
}

int testVectorAddition()
{
    return 1;
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
