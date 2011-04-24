
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

Connection* ConnectionController::doConnect(char *hostname, int port, PacketHandler handler)
{
    if(!handler)
    {
        return NULL;
    }
    Connection *c = new Connection(hostname, port, handler);
    if(!c->getSocket())
    {
        return NULL;
    }
    _connections.push_back(c);
    FD_SET(c->getSocket(), &_fd_master);
    return c;
}

Connection* ConnectionController::doListen(char *port, PacketHandler handler)
{
    if(!handler)
    {
        return NULL;
    }
    Connection *c = new Connection(port, handler);
    if(c->getState() != LISTEN)
    {
        return NULL;
    }
    _listenconns.push_back(c);
    FD_SET(c->getSocket(), &_fd_master);
    return c;
}

void ConnectionController::endListen(Connection *conn)
{
    _listenconns.remove(conn);
    FD_CLR(conn->getSocket(), &_fd_master);
    delete conn;
}

bool ConnectionController::doDisconnect(Connection *conn)
{
    _connections.remove(conn);
    FD_CLR(conn->getSocket(), &_fd_master);
    delete conn;
    return true;
}

bool ConnectionController::doSelect()
{
    list<Connection*>::iterator it;

    FD_ZERO(&_fd_readyforrecv);
    FD_ZERO(&_fd_readyforsend);
    FD_ZERO(&_fd_error);
    int i = 0;

    _fd_readyforrecv = _fd_master;
    //_fd_readyforsend = _fd_master;  // This should be outbound connections in progress
    _fd_error = _fd_master;

    for ( it = _selectme.begin() ; it != _selectme.end() ; it++ )
    {
        FD_CLR((*it)->getSocket(), &_fd_readyforrecv);
        i++;
    }
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    int result = 0;
    if(i > 0)
    {
        result = select(NULL, &_fd_readyforrecv, &_fd_readyforsend, &_fd_error, &tv);
    }
    else
    {
        result = select(NULL, &_fd_readyforrecv, &_fd_readyforsend, &_fd_error, NULL);
    }

    for ( it = _selectme.begin() ; it != _selectme.end() ; it++ )
    {
        FD_SET((*it)->getSocket(), &_fd_readyforrecv);
        result++;
    }
    if(!result || result == SOCKET_ERROR || result == -1)
    {
#if DEBUG
        int error = WSAGetLastError();
#endif
        return false;
    }

    _selectme.clear();
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

int ConnectionController::doAccept()
{
    list<Connection*>::iterator it;
    int r = 0;
    for ( it = _listenconns.begin() ; it != _listenconns.end() ; it++ )
    {
        if(FD_ISSET((*it)->getSocket(), &_fd_readyforrecv))
        {
            Connection *t = (*it)->doAccept();
            if(t)
            {
                if (t->getState() == ACCEPTING)
                {
                    delete t;
                }
                else
                {
                    _connections.push_back(t);
                    FD_SET(t->getSocket(), &_fd_master);
                    r++;
                }
            }
        }
    }
    return r;
}

int ConnectionController::doRecv()
{
    list<Connection*>::iterator it;
    list<Connection*> tbd;

    int received = 0;
    int i;

    //Connection *c = NULL;
    for ( it = _connections.begin() ; it != _connections.end() ; it++ )
    {
        if(FD_ISSET((*it)->getSocket(), &_fd_readyforrecv))
        {
            if((i = (*it)->doRecv()) < 0)
                tbd.push_back(*it);
            else if(i==0)
                continue;
            else
            {
                received++;
                if(i>1) _selectme.push_back(*it);
            }
        }
    }
    for ( it = tbd.begin() ; it != tbd.end() ; it ++ )
    {
        FD_CLR((*it)->getSocket(), &_fd_master);
        _connections.remove(*it);
        delete *it;
    }

    return received;
}

int ConnectionController::doRouting()
{
    list<Connection*>::iterator it;
    int i = 0;

    for ( it = _connections.begin() ; it != _connections.end() ; it++ )
    {
        i += (*it)->doRouting();
    }
    return i;
}
