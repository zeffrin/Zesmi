

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

        /*

        // Change the socket mode on the listening socket from blocking to non-block

        if(_connstate != CONNERROR)
        {
            unsigned long NonBlock = 1;
            if (ioctlsocket(sock, FIONBIO, &NonBlock) == SOCKET_ERROR)
            {
                log->writeToLog("ioctlsocket() failed \n");
                closesocket(sock);
                _connstate = CONNERROR;
            }
        }

        */

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
    t = accept(sock, NULL, NULL);
    if(t == INVALID_SOCKET)
    {
        _connstate = CONNERROR;

        LogSocketError();
        return NULL;
    }
    _connstate = OPEN;
    Connection *r = new Connection(t);
    return r;

}

bool Connection::doRecv()
{
    #define RECV_BUF 256 // TODO put this somewhere better

    char buf[RECV_BUF];

    bool goagain = true;


    while(goagain)
    {
        int i = recv(sock, buf, i, 0);
        if ( i > RECV_BUF )
        {
            i = RECV_BUF;
            goagain = true;
        }
        else
        {
            goagain = false;
        }
        if(i <= 0)
        {
            log->writeToLog("Socket closed\n");
            goagain = false;
            return false;
        }
        strncat(_sockbuf, buf, i);
        // translate stream into Packets and store in connection
        if( ( strlen(_sockbuf) ) < sizeof(byte))
        {
            continue;
        }

        Packet *t = new Packet();
        sscanf(_sockbuf, "%1c", &(t->PacketID));
        switch(t->PacketID)
        {
            default:
            {
                if(strlen(_sockbuf) < sizeof(PlayerID))
                {
                    continue;
                }
                PlayerID *p = (PlayerID *) t;
                sscanf(_sockbuf + 2, "%c%64s%64s%c", &(p->ProtocolVersion), *(&(p->Username)), *(&(p->VerificationKey)), &(p->Unused));
                *_sockbuf = '\0';

                //sprintf(_sockbuf, "%c\n%s\n%s", &(p->ProtocolVersion), *(&(p->Username)), *(&(p->VerificationKey)));
                //log->writeToLog(_sockbuf);
                //*_sockbuf = '\0';

                // TODO not be faking this here
                ServerID s;
                s.PacketID = 0x00;
                s.ProtocolVersion = p->ProtocolVersion;
                strncpy(s.ServerMOTD, "Black Sheep        Black Sheep        Black Sheep              ", 64);
                strncpy(s.ServerName, "Zeff                                                           ", 64);
                s.UserType = 0x01;

                SendPacket((Packet *)&s, PLAYERID);
                break;

            }
            //default:
            //    *_sockbuf = '\0';  // clear the sockbuf

        }


        // TODO store left over peices back in sockbuf



    }

    return true;


}

void Connection::LogSocketError()
{
    switch(WSAGetLastError())
    {
        case WSANOTINITIALISED: log->writeToLog("A successful WSAStartup call must occur before using this function.\n");
                                break;
        case WSAECONNRESET:     log->writeToLog("An incoming connection was indicated, but was subsequently terminated by the remote peer prior to accepting the call.\n");
                                break;
        case WSAEFAULT:         log->writeToLog("The addrlen parameter is too small or addr is not a valid part of the user address space.\n");
                                break;
        case WSAEINTR:          log->writeToLog("A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall.\n");
                                break;
        case WSAEINVAL:         log->writeToLog("The listen function was not invoked prior to accept.\n");
                                break;
        case WSAEINPROGRESS:    log->writeToLog("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.\n");
                                break;
        case WSAEMFILE:         log->writeToLog("The queue is nonempty upon entry to accept and there are no descriptors available.\n");
                                break;
        case WSAENETDOWN:       log->writeToLog("The network subsystem has failed.\n");
                                break;
        case WSAENOBUFS:        log->writeToLog("No buffer space is available.\n");
                                break;
        case WSAENOTSOCK:       log->writeToLog("The descriptor is not a socket.\n");
                                break;
        case WSAEOPNOTSUPP:     log->writeToLog("The referenced socket is not a type that supports connection-oriented service.\n");
                                break;
        case WSAEWOULDBLOCK:    log->writeToLog("The socket is marked as nonblocking and no connections are present to be accepted.\n");
                                break;
        default:
                                log->writeToLog("SOCKET Unknown error\n");
                                break;
    }
}

void Connection::SendPacket(const Packet *p, PacketType pt) // TODO Make PacketType enum right, and have a matching size array
{
    char buf[1092];
    switch(pt)
    {
        case PLAYERID:
                        ServerID *s = (ServerID *)p;
                        sprintf(buf, "%1uc%1uc%s%s%1uc", s->PacketID, s->ProtocolVersion, s->ServerName, s->ServerMOTD, s->UserType);
                        log->writeToLog(buf);
                        int r = send(sock, buf, 131, 0);
                        break;
    }


}
