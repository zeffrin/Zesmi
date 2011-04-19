

#include <stdio.h>
#include "connection.hpp"

Connection::Connection()
{
    // For handling an in progress connection

    sock = NULL;
    _connstate = ACCEPTING;
    _handlePacket = NULL;
    *_sockbuf = '\0';
    _sockbuflen = 0;

}

Connection::Connection(char *hostname, int port, PacketHandler handler)
{
    *_sockbuf = '\0';
    _sockbuflen = 0;
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
    if ((addr.sin_addr.s_addr = inet_addr(hostname)) == (unsigned)-1)
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
    *_sockbuf = '\0';
    _sockbuflen = 0;
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
    *_sockbuf = '\0';
    _sockbuflen = 0;
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

int Connection::doRecv()
{
    char buf[RECV_BUF];
    bool unknown = false;

    if(_sockbuflen > 0) {
       strncpy(buf, _sockbuf, _sockbuflen); }
    int j = 0;
    j = recv(sock, buf, RECV_BUF - (1 + j), 0);
    int i = j;
    //buf[i-1] = '\0';
    if(i <= 0)
        return -1;

    Packet *p = NULL;
    int packetid = buf[0];
    switch(packetid)
    {
        case P_KEEPALIVE: // TODO handle endianess
        {
            KeepAlivePacket *t = new KeepAlivePacket;
            t->PacketID = P_KEEPALIVE;
            i -= sizeof(KeepAlivePacket);
            p = (Packet*)t;
            break;
        }
        case P_LOGIN:
        {
            LoginPacket *t = new LoginPacket;
            *t = *((LoginPacket*)&buf);
            i -= sizeof(LoginPacket);
            p = (Packet*)t;
            break;
        }
        case P_HANDSHAKE:
        {
            HandShakePacket *t = new HandShakePacket;
            *t = *((HandShakePacket*)&buf);
            i -= sizeof(HandShakePacket);
            p = (Packet*)t;
            break;
        }
        case P_CHAT:
        {
            ChatPacket *t = new ChatPacket;
            *t = *((ChatPacket*)&buf);
            i -= sizeof(ChatPacket);
            p = (Packet*)t;
            break;
        }
        case P_UPDATETIME:
        {
            UpdateTimePacket *t = new UpdateTimePacket;
            *t = *((UpdateTimePacket*)&buf);
            i -= sizeof(UpdateTimePacket);
            p = (Packet*)t;
            break;
        }
        case P_PLAYERINVENTORY:
        {
            PlayerInventoryPacket *t = new PlayerInventoryPacket;
            *t = *((PlayerInventoryPacket*)&buf);
            i -= sizeof(PlayerInventoryPacket);
            p = (Packet*)t;
            break;
        }
        case P_PLAYERLOOKMOVE:
        {
            PlayerLookMovePacket *t = new PlayerLookMovePacket;
            *t = *((PlayerLookMovePacket*)&buf);
            i -= sizeof(PlayerLookMovePacket);
            p = (Packet*)t;
            break;
        }
        default:
        {
            *_sockbuf = '\0';  // TODO unknown packet should cause disconnect
            i = 0;
            p = NULL;
            unknown = true;
            break;
        }
    }

    if(i > 0)
    {
        memcpy(_sockbuf, buf+(j-i), i);
        _sockbuflen = i;
    }

    else
        *_sockbuf = '\0';

    if(!unknown)
    {
        inMessages.push_back(p);
        return 1;
    }
    return 0;

}

int Connection::doRouting()
{
    Packet* it;
    int i = 0;

    while(!inMessages.empty())
    {
        it = inMessages.front();
        if(_handlePacket(it, this)) i++;
        inMessages.pop_front();
        delete it;
    }
    return i;
}

bool Connection::SendPacket(const Packet *p)  // TODO rolling out this many pointers on the stack sucks...  make a data structure holding a pointer of each type for reuse
{
    char buf[1092];
    int bf = 0;  // used buffer length
    switch(p->PacketID)
    {
        case P_KEEPALIVE: // TODO handle endianess
            *buf = '\0';
            break;
        case P_LOGIN:
        {
            bf = sizeof(LoginPacket);
            *((LoginPacket*)buf) = *((LoginPacket*)p);
            break;
        }
        case P_HANDSHAKE:
        {
            bf = sizeof(HandShakePacket);
            *((HandShakePacket*)buf) = *((HandShakePacket*)p);
            break;
        }
        case P_CHAT:
        {
            bf = sizeof(ChatPacket);
            *((ChatPacket*)buf) = *((ChatPacket*)p);
            break;
        }
        case P_UPDATETIME:
        {
            bf = sizeof(UpdateTimePacket);
            *((UpdateTimePacket*)buf) = *((UpdateTimePacket*)p);
            break;
        }
        case P_PLAYERINVENTORY:
        {
            bf = sizeof(PlayerInventoryPacket);
            *((PlayerInventoryPacket*)buf) = *((PlayerInventoryPacket*)p);
            break;
        }
        case P_SPAWNPOSITION:
        {
            bf = sizeof(SpawnPositionPacket);
            *((SpawnPositionPacket*)buf) = *((SpawnPositionPacket*)p);
            break;
        }
        case P_ACTION:
        {
            bf = sizeof(ActionPacket);
            *((ActionPacket*)buf) = *((ActionPacket*)p);
            break;
        }
        case P_HEALTH:
        {
            bf = sizeof(HealthPacket);
            *((HealthPacket*)buf) = *((HealthPacket*)p);
            break;
        }
        case P_RESPAWN:
        {
            bf = sizeof(RespawnPacket);
            *((RespawnPacket*)buf) = *((RespawnPacket*)p);
            break;
        }
        case P_FLYING:
        {
            bf = sizeof(FlyingPacket);
            *((FlyingPacket*)buf) = *((FlyingPacket*)p);
            break;
        }
        case P_PLAYERPOSITION:
        {
            bf = sizeof(PlayerPositionPacket);
            *((PlayerPositionPacket*)buf) = *((PlayerPositionPacket*)p);
            break;
        }
        case P_PLAYERLOOK:
        {
            bf = sizeof(PlayerLookPacket);
            *((PlayerLookPacket*)buf) = *((PlayerLookPacket*)p);
            break;
        }
        case P_PLAYERLOOKMOVE:
        {
            bf = sizeof(PlayerLookMovePacket);
            *((PlayerLookMovePacket*)buf) = *((PlayerLookMovePacket*)p);
            break;
        }
        case P_BLOCKDIG:
        {
            bf = sizeof(BlockDigPacket);
            *((BlockDigPacket*)buf) = *((BlockDigPacket*)p);
            break;
        }
        case P_PLACE:
        {
            bf = sizeof(PlacePacket);
            *((PlacePacket*)buf) = *((PlacePacket*)p);
            break;
        }
        case P_BLOCKITEMSWITCH:
        {
            bf = sizeof(BlockItemSwitchPacket);
            *((BlockItemSwitchPacket*)buf) = *((BlockItemSwitchPacket*)p);
            break;
        }
        case P_SLEEP:
        {
            bf = sizeof(SleepPacket);
            *((SleepPacket*)buf) = *((SleepPacket*)p);
            break;
        }
        case P_ARMANIMATION:
        {
            bf = sizeof(ArmAnimationPacket);
            *((ArmAnimationPacket*)buf) = *((ArmAnimationPacket*)p);
            break;
        }
        case P_PACKET19:
        {
            bf = sizeof(P19Packet);
            *((P19Packet*)buf) = *((P19Packet*)p);
            break;
        }
        case P_NAMEDENTITYSPAWN:
        {
            bf = sizeof(NamedEntitySpawnPacket);
            *((NamedEntitySpawnPacket*)buf) = *((NamedEntitySpawnPacket*)p);
            break;
        }
        case P_PICKUPSPAWN:
        {
            bf = sizeof(PickupSpawnPacket);
            *((PickupSpawnPacket*)buf) = *((PickupSpawnPacket*)p);
            break;
        }
        case P_COLLECT:
        {
            bf = sizeof(CollectPacket);
            *((CollectPacket*)buf) = *((CollectPacket*)p);
            break;
        }
        case P_VEHICLESPAWN:
        {
            bf = sizeof(VehicleSpawnPacket);
            *((VehicleSpawnPacket*)buf) = *((VehicleSpawnPacket*)p);
            break;
        }
        case P_MOBSPAWN:
        {
            bf = sizeof(MobSpawnPacket);
            *((MobSpawnPacket*)buf) = *((MobSpawnPacket*)p);
            break;
        }
        case P_TITLE:
        {
            bf = sizeof(TitlePacket);
            *((TitlePacket*)buf) = *((TitlePacket*)p);
            break;
        }
        default:
            *buf = '\0';
            bf = 0;
            return false;
    }
    send(sock, buf, bf + 1, 0); // need to do error checking
    return true;
}


