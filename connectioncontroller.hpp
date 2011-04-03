#ifndef CONNECTIONCONTROLLER_HPP_INCLUDED
#define CONNECTIONCONTROLLER_HPP_INCLUDED

#include <list>

#include "connection.hpp"

/* Handles connections to other servers and players
   handles sending and receiving packets and routing
   them to appropriate server
*/

using namespace std;

class ConnectionController
{
    public:
        ConnectionController();
        ~ConnectionController();
        void startListen(char *port);
        void stopListen(Connection *conn);
        //Connection* connect(short int *port);
        void doSelect();
        void doAccept();
        void doSend();
        void doRecv();
        void doError();
        void doRouting();

    protected:
        list<Connection*> _connections;
        list<Connection*> _listenconns;

        fd_set _fd_readyforsend;
        fd_set _fd_readyforrecv;
        fd_set _fd_error;

};
#endif // CONNECTIONCONTROLLER_HPP_INCLUDED
