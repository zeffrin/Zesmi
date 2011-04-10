
#include "connectioncontroller.hpp"
#include "logger.hpp"

ConnectionController* ConnectionController::_instance = NULL;

static Logger::Logger *log = Logger::getInstance();

ConnectionController::ConnectionController()
{
    log->writeToLog("Spawning ConnectionController");
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
        // TODO: send to logger
        return NULL;
    }
    _listenconns.push_back(c);
    return c;
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

    int fdmax = 0;

    // TODO Keep a master FD_SET and copy it in instead of setting like this

    for ( it = _connections.begin() ; it != _connections.end() ; it++ )
    {
        if((*it)->getSocket() > fdmax)
        {
             fdmax = (*it)->getSocket();
        }
        FD_SET((*it)->getSocket(), &_fd_error);
        FD_SET((*it)->getSocket(), &_fd_readyforrecv);
        FD_SET((*it)->getSocket(), &_fd_readyforsend);

        //assert(_fd_readyforrecv != NULL));

    }
    for ( it = _listenconns.begin() ; it != _listenconns.end() ; it++ )
    {
        if((*it)->getSocket() > fdmax)
        {
             fdmax = (*it)->getSocket();
        }
        FD_SET((*it)->getSocket(), &_fd_readyforrecv);
    }

    int result = select(fdmax+1, &_fd_readyforrecv, &_fd_readyforsend, &_fd_error, NULL);
    if(!result || result == SOCKET_ERROR)
    {
        log->writeToLog("not result from Select in connection controller\n");
    }

}

void ConnectionController::doAccept()
{
    list<Connection*>::iterator it;
    for ( it = _listenconns.begin() ; it != _listenconns.end() ; it++ )
    {
        if(FD_ISSET((*it)->getSocket(), &_fd_readyforrecv))
        {
            _connections.push_back((*it)->doAccept());
            log->writeToLog("Accepted connection.\n");

        }
    }
}

void ConnectionController::doRecv()
{
    list<Connection*>::iterator it;
    //Connection *c = NULL;
    for ( it = _connections.begin() ; it != _connections.end() ; it++ )
    {
        if(FD_ISSET((*it)->getSocket(), &_fd_readyforrecv))
        {
            log->writeToLog("Receiving data\n");
            (*it)->doRecv();
        }
    }
}
