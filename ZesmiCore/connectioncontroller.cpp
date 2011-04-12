
#include "connectioncontroller.hpp"

ConnectionController* ConnectionController::_instance = NULL;

ConnectionController::ConnectionController()
{
    FD_ZERO(&_fd_master);
}

ConnectionController::~ConnectionController()
{
    Connection *c;
    while(_connections.size())
    {
        c = _connections.back();
        _connections.pop_back();
        FD_CLR(c->getSocket(), &_fd_master);
        delete c;

    }
    while(_listenconns.size())
    {
        c = _listenconns.back();
        _listenconns.pop_back();
        FD_CLR(c->getSocket(), &_fd_master);
        delete c;

    }
}

ConnectionController* ConnectionController::getInstance()
{
    if(_instance)
        return _instance;
    _instance = new ConnectionController();
    return _instance;

}

Connection* ConnectionController::startListen(char *port)
{
    Connection *c = new Connection(port);
    if(!c)
    {
        return NULL;
    }
    _listenconns.push_back(c);
    FD_SET(c->getSocket(), &_fd_master);
    return c;
}

void ConnectionController::stopListen(Connection *conn)
{
    _listenconns.remove(conn);
    FD_CLR(conn->getSocket(), &_fd_master);
    delete conn;
}

bool ConnectionController::doSelect()
{
    list<Connection*>::iterator it;

    FD_ZERO(&_fd_readyforrecv);
    FD_ZERO(&_fd_readyforsend);
    FD_ZERO(&_fd_error);

    _fd_readyforrecv = _fd_master;
    _fd_readyforsend = _fd_master;
    _fd_error = _fd_master;

    // TODO fdmax is ignored according to msdn, but it fixed my trouble before I think
    int result = select(NULL, &_fd_readyforrecv, &_fd_readyforsend, &_fd_error, NULL);
    if(!result || result == SOCKET_ERROR)
    {
        return false;
    }
    return true;

}

void ConnectionController::doError()
{
    list<Connection*>::iterator it;

    for ( it = _connections.begin() ; it != _connections.end() ; it++ )
    {
        if(FD_ISSET((*it)->getSocket(), &_fd_error))
        {
            it = _connections.erase(it);
            it--;
        }

    }
}

void ConnectionController::doAccept()
{
    list<Connection*>::iterator it;
    for ( it = _listenconns.begin() ; it != _listenconns.end() ; it++ )
    {
        if(FD_ISSET((*it)->getSocket(), &_fd_readyforrecv))
        {
            Connection *t = (*it)->doAccept();
            if(t)
            {
                _connections.push_back(t);
                FD_SET(t->getSocket(), &_fd_master);
            }
        }
    }
}

void ConnectionController::doRecv()
{
    list<Connection*>::iterator it;
    list<Connection*> tbd;


    //Connection *c = NULL;
    for ( it = _connections.begin() ; it != _connections.end() ; it++ )
    {
        if(FD_ISSET((*it)->getSocket(), &_fd_readyforrecv))
        {
            if(!(*it)->doRecv())
            {
                tbd.push_back(*it);
            }
        }
    }
    for ( it = tbd.begin() ; it != tbd.end() ; it ++ )
    {
        FD_CLR((*it)->getSocket(), &_fd_master);
        _connections.remove(*it);
        delete *it;
    }
}

