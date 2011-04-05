
#include "initialize.hpp"
#include "logger.hpp"
#include "connectioncontroller.hpp"

using namespace std;

int main(void)
{
    Initialize *init = new Initialize();
    bool result = init->doInitialization();
    Logger *log = Logger::getInstance();

    if(!result)
    {
        // TODO send to logger
    }

    ConnectionController *conns = ConnectionController::getInstance();
    if(conns->startListen("1022"))
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
        conns->doAccept();
        //conns->doRecv();
        //conns->doRouting();
        //conns->doSend();
    }

    delete log;
    delete conns;
    delete init;


}
