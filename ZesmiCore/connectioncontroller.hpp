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

        static ConnectionController *getInstance();
        Connection* doListen(char *port, PacketHandler handler);
        void endListen(Connection *conn);
        Connection* doConnect(char *host, int port, PacketHandler handler);
        bool doDisconnect(Connection *conn);
        bool doSelect();
        int doAccept();
        void doSend();
        int doRecv();
        int doRouting();
        void doError();
        ~ConnectionController();

    protected:
        ConnectionController();


        static ConnectionController *_instance;

        list<Connection*> _connections;
        list<Connection*> _listenconns;

        fd_set _fd_master;

        fd_set _fd_readyforsend;
        fd_set _fd_readyforrecv;
        fd_set _fd_error;

};
#endif // CONNECTIONCONTROLLER_HPP_INCLUDED
