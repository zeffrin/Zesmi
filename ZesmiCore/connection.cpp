

#include <stdio.h>
#include "connection.hpp"

Connection::Connection()
{
    // For handling an in progress connection

    sock = NULL;
    _connstate = ACCEPTING;
    _handlePacket = NULL;

}

Connection::Connection(char *hostname, int port, PacketHandler handler)
{

    if(!handler)
    {
        _connstate = CONNERROR;
        sock = NULL;
        _handlePacket = NULL;
        return;
    }
    _handlePacket = handler;
    sock = INVALID_SOCKET;
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        sock = NULL;
        _connstate = CONNERROR;
        return;
    }

    SOCKADDR_IN addr;
    addr.sin_family     = AF_INET;
    addr.sin_port       = htons(port);
    if ((addr.sin_addr.s_addr = inet_addr(hostname)) == -1)
    {
        struct hostent *hs;
        if ((hs = gethostbyname(hostname)) == NULL)
        {
            sock = NULL;
            _connstate = CONNERROR;
            return;
        }
        addr.sin_family = hs->h_addrtype;
        memcpy((void *)&addr.sin_addr.s_addr, hs->h_addr, hs->h_length);
    }
    if (connect(sock, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR)
    {

        int error = WSAGetLastError();

        sock = NULL;
        _connstate = CONNERROR;
        return;
    }

    u_long iMode=1;
    ioctlsocket(sock, FIONBIO,&iMode);

    _connstate = OPEN;
}

Connection::Connection(char *port, PacketHandler handler)
{
    // listen socket

    #ifdef TARGET_OS_MAC
    #error "TODO sockets lib for osx"
#elif defined __linux__
    #error "TODO sockets for linux"
#elif defined _WIN32 || defined _WIN64

    _connstate = NEW;
    if(!handler)
    {
        _connstate = CONNERROR;
        return;
    }
    _handlePacket = handler;

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
        _connstate = CONNERROR;
    }
    else
    {
        sock = INVALID_SOCKET;
        sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if(sock == INVALID_SOCKET)
        {
            sock = NULL;
            _connstate = CONNERROR;
        }

        iresult = bind(sock, result->ai_addr, (int)result->ai_addrlen);
        if(iresult == SOCKET_ERROR)
        {
            closesocket(sock);
            sock = NULL;
            _connstate = CONNERROR;
        }

        // Dont need this anymore
        freeaddrinfo(result);

        /* Set up queue for incoming connections. */
        if(_connstate != CONNERROR)
        {
            _connstate = LISTEN;
            listen(sock, 5);
        }

        u_long iMode=1;
        ioctlsocket(sock, FIONBIO,&iMode);

    }

#else
    #error "unknown platform"
#endif

}

Connection::Connection(SOCKET s, PacketHandler handler)
{
    //For use accepting connections
    sock = s;
    u_long iMode=1;
    ioctlsocket(sock, FIONBIO,&iMode);
    _connstate = OPEN;
    _handlePacket = handler;
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
            sock = NULL;
        #endif
        _connstate = CLOSED;

    }
    _handlePacket = NULL;

}

bool Connection::isListener()
{
    return (_connstate == LISTEN);
}

SOCKET Connection::getSocket()
{
    return sock;
}

ConnectionState Connection::getState()
{
    return _connstate;
}

Connection* Connection::doAccept()
{
    SOCKET t;
    t = accept(sock, NULL, NULL);

    if(t == INVALID_SOCKET)
    {
        if(WSAGetLastError() == WSAEWOULDBLOCK)
        {
            Connection *r = new Connection();
            return r;
        }
        else
        {
            _connstate = CONNERROR;
            return NULL;
        }
    }
    _connstate = OPEN;
    Connection *r = new Connection(t, _handlePacket);
    return r;

}

bool Connection::doRecv()
{
    #define RECV_BUF 256 // TODO put this somewhere better

    char buf[RECV_BUF];
    char *cp;

    bool goagain = true;


    while(goagain)
    {
        int i = recv(sock, buf, RECV_BUF - 1, 0);
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
            goagain = false;
            return false;
        }
        strncat(_sockbuf, buf, i);

        // Get Packet ID
        char packetid;
        sscanf(_sockbuf, "%c", &packetid);
        cp = _sockbuf;  // set cp to position in sockbuf


        char tmp[30];
        sprintf(tmp, "Received packet: %d\n", packetid);

        //TODO will want to loop through buffer and leave remainder somewhere
        Packet *p = NULL;

        switch(packetid)
        {
            case P_KEEPALIVE:
            {
                KeepAlive k;
                k.PacketID = 0;
                SendPacket((Packet*)&k); // TODO remove testing, packets should be handled in consumer processes
                break;
            }
            case P_LOGIN:
            {
                Login *t = new Login;
                // TODO if args from sscanf correct bla if not bla
                sscanf(cp,"%c%d%64s%64s%ld%c", &(t->PacketID), &(t->ProtocolVersion),t->Username, t->VerificationKey, &(t->MapSeed), &(t->Dimension));
                cp += sizeof(Login);
                p = (Packet*)t;
                break;
            }
            case P_HANDSHAKE:
            {
                HandShake *t = new HandShake;
                // TODO if args from sscanf correct bla if not bla
                sscanf(cp, "%c%s", &(t->PacketID), t->Username);
                cp += sizeof(HandShake);
                p = (Packet*)t;
                break;

            }
            case P_PLAYERLOOKMOVE:
            {
                PlayerLookMove *t = new PlayerLookMove;
                // TODO if args from sscanf correct bla if not bla
                sscanf(cp, "%c%lf%lf%lf%lf%f%f", &(t->PacketID), &(t->X), &(t->Y), &(t->Stance), &(t->Z), &(t->Yaw), &(t->Pitch) );
                cp += sizeof(PlayerLookMove);
                p = (Packet*)t;
                break;
            }
            default:
            {
                *_sockbuf = '\0';  // TODO unknown packet should cause disconnect
                p = NULL;
                break;
            }
        }

        if(p)
        {
            inMessages.push_back(p);
        }

        // TODO store left over peices back in sockbuf
    }

    return true;
}

int Connection::doRouting()
{
    list<Packet*>::iterator it;
    int i;

    for( it = inMessages.begin() ; it != inMessages.end() ; it++ )
    {
        _handlePacket(*it);
        i++;
    }
    return i;
}

bool Connection::SendPacket(const Packet *p)
{
    char buf[1092];
    switch(p->PacketID)
    {
        case P_KEEPALIVE:
            *buf = '\0';
            break;
        case P_HANDSHAKE:
            HandShake *t;
            t = (HandShake*)p;
            sprintf(buf, "%c%-64s", t->PacketID, t->Username);
            break;
        default:
            *buf = '\0';
            return false;
    }
    send(sock, buf, strlen(buf) + 1, 0); // need to do error checking
    sprintf(buf, "%d%s\n", p->PacketID, buf);
    return true; // TODO error checking
}


