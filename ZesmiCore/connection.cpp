

#include <stdio.h>
#include "connection.hpp"
#include "logger.hpp"

static Logger::Logger *log = Logger::getInstance();

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
        log->writeToLog("Unable to resolve localaddress and port\n");
        _connstate = CONNERROR;
    }
    else
    {

        sock = INVALID_SOCKET;
        sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

        if(sock == INVALID_SOCKET)
        {
            log->writeToLog("Error: Invalid socket.\n");
            freeaddrinfo(result);
            _connstate = CONNERROR;
        }

        iresult = bind(sock, result->ai_addr, (int)result->ai_addrlen);
        if(iresult == SOCKET_ERROR)
        {
            log->writeToLog("Error: Unable to bind to port.\n");
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
        log->writeToLog("Listening on Port x.\n");
    }

#else
    #error "unknown platform"
#endif

}

Connection::Connection(SOCKET s)
{
    sock = s;
    _connstate = OPEN;
}

Connection::~Connection()
{
    if(_connstate != CLOSED)
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

}


/*  TODO When I know for sure how packets are going to work out

void Connection::send(const Block *block)
{
    return;
}

void Connection::send(const Packet *packet)
{
    return;
}

*/

bool Connection::isListener()
{
    return (_connstate == LISTEN);
}

SOCKET Connection::getSocket()
{
    return sock;
}

Connection* Connection::doAccept()
{
    SOCKET t;
    t = accept(sock, &_clientaddr, &_clientaddrlen);
    if(!t)
    {
        _connstate = CONNERROR;
        log->writeToLog("Error while accepting connection\n");
        return NULL;
    }
    _connstate = OPEN;
    Connection *r = new Connection(t);
    return r;

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
            log->writeToLog("Error reading in doRecv goagain loop\n");
        }
        if ( i > RECV_BUF )
        {
            i = RECV_BUF;
            goagain = true;
        }
        i = recv(sock, buf, i, MSG_PARTIAL);
        strncat(_sockbuf, buf, i);
        // translate stream into Packets and store in connection
        if( ( strlen(_sockbuf) ) < sizeof(byte))
        {
            continue;
        }

        Packet *t = new Packet();
        sscanf(_sockbuf, "%1uc", &(t->PacketID));
        switch(t->PacketID)
        {
            case 0x00:
            {
                if(strlen(_sockbuf) < sizeof(PlayerID))
                {
                    continue;
                }
                PlayerID *p = (PlayerID *) t;
                sscanf(_sockbuf + 2, "%1uc%64s%64s%1uc", &(p->ProtocolVersion), &(p->Username), &(p->VerificationKey), &(p->Unused));
                log->writeToLog(_sockbuf);
                *_sockbuf = '\0';

            }
            default:
                *_sockbuf = '\0';  // clear the sockbuf

        }


        // store left over peices in _sockbuf

    }




}
