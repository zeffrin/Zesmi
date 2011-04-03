

#include <stdio.h>
#include "connection.hpp"

Connection::Connection(char *hostname, char *port)
{

    _connstate = OPEN;
}

Connection::Connection(char *port)
{
    // listen socket

    #ifdef TARGET_OS_MAC
    #error "TODO sockets lib for osx"
#elif defined __linux__
    #error "TODO sockets for linux"
#elif defined _WIN32 || defined _WIN64

    _connstate = NEW;

    struct addrinfo *result = NULL, hints;  // unused variable from msdn *ptr = NULL;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    //Resolve the local address and port to be used by the server
    int iresult = getaddrinfo(NULL, port, &hints, &result);
    if(iresult != 0)
    {
        // TODO: send error to logger
        // TODO: set connectionstate on this object
    }
    else
    {


        // argh seg fault here why

        sock = INVALID_SOCKET;
        sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

        if(sock == INVALID_SOCKET)
        {
            // TODO: send error to logger
            freeaddrinfo(result);
            _connstate = CONNERROR;
        }

        iresult = bind(sock, result->ai_addr, (int)result->ai_addrlen);
        if(iresult == SOCKET_ERROR)
        {
            //TODO : send error to logger
            freeaddrinfo(result);
            closesocket(sock);
            _connstate = CONNERROR;
        }

        // Dont need this anymore, unsure whether need to remove above as well, meh
        freeaddrinfo(result);

        /* Set up queue for incoming connections. */
        if(_connstate != CONNERROR)
        {
            _connstate = LISTEN;
            listen(sock, 5);
        }
    }

#else
    #error "unknown platform"
#endif

}

Connection::~Connection()
{
#ifdef TARGET_OS_MAC
    #error "TODO sockets lib for osx"
#elif defined __linux__
    #error "TODO sockets for linux"
#elif defined _WIN32 || defined _WIN64
    closesocket(sock);
#endif
    _connstate = CLOSED;

}

void Connection::send(const Block *block)
{
    return;
}

void Connection::send(const Packet *packet)
{
    return;
}

bool Connection::isListener()
{
    return (_connstate == LISTEN);
}

SOCKET Connection::getSocket()
{
    return sock;
}

SOCKET Connection::doAccept()
{
    SOCKET t;
    t = accept(sock, &_clientaddr, &_clientaddrlen);
    if(!t)
    {
        _connstate = CONNERROR;
        return NULL;
    }
    _connstate = OPEN;
    return t;

}

void Connection::doRecv()
{
    #define RECV_BUF 256 // TODO put this somewhere better

    char buf[RECV_BUF];

    if(strlen(_sockbuf) > 0)
    {
        strncpy(buf, _sockbuf, RECV_BUF);
        _sockbuf[0] = '\0';
    }

    bool goagain = true;

    while(goagain)
    {
        int i = recv(sock, buf, 256, MSG_PEEK);
        if(i <= 0)
        {
            // TODO send to logger
        }
        if ( i > RECV_BUF )
        {
            i = RECV_BUF;
            goagain = true;
        }
        i = recv(sock, buf, i, MSG_PARTIAL);

        // translate stream into Packets and store in connection

        Packet *p = new Packet();

        // store left over peices in _sockbuf

    }




}
