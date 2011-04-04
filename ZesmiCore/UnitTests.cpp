
#include "initialize.hpp"
#include "connectioncontroller.hpp"

using namespace std;

int main(void)
{
    Initialize *init = new Initialize();
    bool result = init->doInitialization();
    if(!result)
    {
        // TODO send to logger
    }

    ConnectionController *conns = new ConnectionController();
    conns->startListen("1022");
    while(1)
    {
        conns->doSelect();
        conns->doAccept();
        //conns->doRecv();
        //conns->doRouting();
        //conns->doSend();
    }

    delete conns;
    delete init;


}
