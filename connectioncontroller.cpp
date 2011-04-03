
#include "connectioncontroller.hpp"

ConnectionController::ConnectionController()
{

}

ConnectionController::~ConnectionController()
{
    Connection *c;
    while(_connections.size())
    {
        c = _connections.back();
        _connections.pop_back();
        delete c;

    }
    while(_listenconns.size())
    {
        c = _listenconns.back();
        _listenconns.pop_back();
        delete c;

    }
}

void ConnectionController::startListen(char *port)
{
    Connection *c = new Connection(port);
    if(!c)
    {
        // TODO: send to logger
        return;
    }
    _listenconns.push_back(c);
}

void ConnectionController::stopListen(Connection *conn)
{
    _listenconns.remove(conn);
    delete conn;
}

void ConnectionController::doSelect()
{
    list<Connection*>::iterator it;

    FD_ZERO(&_fd_readyforrecv);
    FD_ZERO(&_fd_readyforsend);
    FD_ZERO(&_fd_error);

    for ( it = _connections.begin() ; it != _connections.end() ; it++ )
    {
        FD_SET((*it)->getSocket(), &_fd_error);
        FD_SET((*it)->getSocket(), &_fd_readyforrecv);
        FD_SET((*it)->getSocket(), &_fd_readyforsend);

        //assert(_fd_readyforrecv != NULL));

    }
    for ( it = _listenconns.begin() ; it != _listenconns.end() ; it++ )
    {
        FD_SET((*it)->getSocket(), &_fd_readyforrecv);
    }

    int result = select(NULL, &_fd_readyforrecv, &_fd_readyforsend, &_fd_error, 0);
    if(!result || result == SOCKET_ERROR)
    {
        //TODO send error to logger
    }

}

void ConnectionController::doAccept()
{
    list<Connection*>::iterator it;
    for ( it = _listenconns.begin() ; it != _listenconns.end() ; it++ )
    {
        if(FD_ISSET((*it), &_fd_readyforrecv))
        {
            (*it)->doAccept();
            _listenconns.remove(*it);
        }
    }
}

void ConnectionController::doRecv()
{
        list<Connection*>::iterator it;
        //Connection *c = NULL;
        for ( it = _connections.begin() ; it != _connections.end() ; it++ )
        {
            (*it)->doRecv();
        }
}
