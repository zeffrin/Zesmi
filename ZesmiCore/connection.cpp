

#include <stdio.h>
#include "connection.hpp"

Connection::Connection()
{
    // For handling an in progress connection

    sock = NULL;
    _connstate = ACCEPTING;
    _handlePacket = NULL;
    *_sockbuf = '\0';

}

Connection::Connection(char *hostname, int port, PacketHandler handler)
{
    *_sockbuf = '\0';
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
    char *cp;
    bool unknown = false;

    int j = strlen(_sockbuf);
    if(j > 0) {
       strncpy(buf, _sockbuf, j); }
    int i = recv(sock, buf, RECV_BUF - (1 + j), 0);
    buf[i] = '\0';
    cp = buf;
    if(i <= 0)
        return -1;

    Packet *p = NULL;
    int packetid = buf[0];
    switch(packetid)
    {
        case P_KEEPALIVE: // TODO handle endianess
        {
            KeepAlivePacket *t = new KeepAlivePacket;
            t->PacketID = 0;
            i -= sizeof(KeepAlivePacket);
            cp += sizeof(KeepAlivePacket);
            p = (Packet*)t;
            break;
        }
        case P_LOGIN:
        {
            LoginPacket *t = new LoginPacket;
            // TODO if args from sscanf correct bla if not bla
            sscanf(cp,"%c%hd%64s%64s%ld%c", &(t->PacketID), &(t->ProtocolVersion),t->Username, t->VerificationKey, &(t->MapSeed), &(t->Dimension));
            i -= sizeof(LoginPacket);
            cp += sizeof(LoginPacket);
            p = (Packet*)t;
            break;
        }
        case P_HANDSHAKE:
        {
            HandShakePacket *t = new HandShakePacket;
            // TODO if args from sscanf correct bla if not bla
            t->PacketID = *cp;
            sscanf(cp + 1, "%s", t->Username);
            i -= sizeof(HandShakePacket) + 1; // TODO Find out why this one is +1 and get rid of DIRTY magic number
            cp += sizeof(HandShakePacket);
            p = (Packet*)t;
            break;
        }
        case P_CHAT:
        {
            ChatPacket *t = new ChatPacket;
            // TODO if args from sscanf ...
            t->PacketID = *cp;
            sscanf(cp+1, "%64s", t->Message);
            i -= sizeof(ChatPacket);
            cp += sizeof(ChatPacket);
            p = (Packet*)t;
            break;
        }
        case P_UPDATETIME:
        {
            UpdateTimePacket *t = new UpdateTimePacket;
            //TODO sscanf..
            t->PacketID = *cp;
            sscanf(cp+1, "%ld", &(t->Time));
            i -= sizeof(UpdateTimePacket);
            cp += sizeof(UpdateTimePacket);
            p = (Packet*)t;
            break;
        }
        case P_PLAYERINVENTORY:
        {
            PlayerInventoryPacket *t = new PlayerInventoryPacket;
            t->PacketID = *cp;
            sscanf(cp+1, "%d%hd%hd%hd", &(t->EntityID), &(t->Slot), &(t->ItemID), &(t->ItemDamage));
            i -= sizeof(PlayerInventoryPacket);
            cp += sizeof(UpdateTimePacket);
            p = (Packet*)t;
            break;
        }
        case P_PLAYERLOOKMOVE:
        {
            PlayerLookMovePacket *t = new PlayerLookMovePacket;
            // TODO if args from sscanf correct bla if not bla
            sscanf(cp, "%c%lf%lf%lf%lf%f%f", &(t->PacketID), &(t->X), &(t->Y), &(t->Stance), &(t->Z), &(t->Yaw), &(t->Pitch) );
            i -= sizeof(PlayerLookMovePacket);
            cp += sizeof(PlayerLookMovePacket);
            p = (Packet*)t;
            break;
        }
        default:
        {
            *_sockbuf = '\0';  // TODO unknown packet should cause disconnect
            i = 0;
            cp = NULL;
            p = NULL;
            unknown = true;
            break;
        }
    }

    if(i > 0)
        strncpy(_sockbuf, cp, i);
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
        {
            inMessages.pop_front();
            delete it;
        }
    }
    return i;
}

bool Connection::SendPacket(const Packet *p)  // TODO rolling out this many pointers on the stack sucks...  make a data structure holding a pointer of each type for reuse
{
    char buf[1092];
    switch(p->PacketID)
    {
        case P_KEEPALIVE: // TODO handle endianess
            *buf = '\0';
            break;
        case P_LOGIN:
        {
            LoginPacket *t;
            t = (LoginPacket*)p;
            sprintf(buf, "%c%hd%-64s%-64s%ld%c", t->PacketID, t->ProtocolVersion, t->Username, t->VerificationKey, t->MapSeed, t->Dimension);
            break;
        }
        case P_HANDSHAKE:
        {
            HandShakePacket *t2;
            t2 = (HandShakePacket*)p;
            sprintf(buf, "%c%-64s", t2->PacketID, t2->Username);
            break;
        }
        case P_CHAT:
        {
            ChatPacket *t3;
            t3 = (ChatPacket*)p;
            sprintf(buf, "%c%-64s", t3->PacketID, t3->Message);
            break;
        }
        case P_UPDATETIME:
        {
            UpdateTimePacket *t4;
            t4 = (UpdateTimePacket*)p;
            sprintf(buf, "%c%ld", t4->PacketID, t4->Time);
            break;
        }
        case P_PLAYERINVENTORY:
        {
            PlayerInventoryPacket *t5;
            t5 = (PlayerInventoryPacket*)p;
            sprintf(buf, "%c%d%hd%hd%hd", t5->PacketID, t5->EntityID, t5->Slot, t5->ItemID, t5->ItemDamage);
            break;
        }
        case P_SPAWNPOSITION:
        {
            SpawnPositionPacket *t6;
            t6 = (SpawnPositionPacket*)p;
            sprintf(buf, "%c%d%d%d", t6->PacketID, t6->X, t6->Y, t6->Z);
            break;
        }
        case P_ACTION:
        {
            ActionPacket *t7;
            t7 = (ActionPacket*)p;
            sprintf(buf, "%c%d%d%c", t7->PacketID, t7->PlayerEntity, t7->TargetEntity, t7->IsLeftClick);
            break;
        }
        case P_HEALTH:
        {
            HealthPacket *t8;
            t8 = (HealthPacket*)p;
            sprintf(buf, "%c%hd", t8->PacketID, t8->Health);
            break;
        }
        case P_RESPAWN:
        {
            RespawnPacket *t9;
            t9 = (RespawnPacket*)p;
            sprintf(buf, "%c", t9->PacketID);
            break;
        }
        case P_FLYING:
        {
            FlyingPacket *t10;
            t10 = (FlyingPacket*)p;
            sprintf(buf, "%c%d", t10->PacketID, t10->Flying);
            break;
        }
        case P_PLAYERPOSITION:
        {
            PlayerPositionPacket *t11;
            t11 = (PlayerPositionPacket*)p;
            sprintf(buf, "%c%lf%lf%lf%lf%d", t11->PacketID, t11->X, t11->Y, t11->Stance, t11->Z, t11->Flying);
            break;
        }
        case P_PLAYERLOOK:
        {
            PlayerLookPacket *t12;
            t12 = (PlayerLookPacket*)p;
            sprintf(buf, "%c%f%f%d", t12->PacketID, t12->Yaw, t12->Pitch, t12->Flying);
            break;
        }
        case P_PLAYERLOOKMOVE:
        {
            PlayerLookMovePacket *t13;
            t13 = (PlayerLookMovePacket*)p;
            sprintf(buf, "%c%lf%lf%lf%lf%f%f%d", t13->PacketID, t13->X, t13->Y, t13->Stance, t13->Z, t13->Yaw, t13->Pitch, t13->Flying);
            break;
        }
        case P_BLOCKDIG:
        {
            BlockDigPacket *t14;
            t14 = (BlockDigPacket*)p;
            sprintf(buf, "%c%d%d%d%d%d", t14->PacketID, t14->Status, t14->X, t14->Y, t14->Z, t14->Face);
            break;
        }
        case P_PLACE:
        {
            PlacePacket *t15;
            t15 = (PlacePacket*)p;
            sprintf(buf, "%c%d%d%d%d%hd%c%hd", t15->PacketID, t15->X, t15->Y, t15->Z, t15->Direction, t15->ItemID, t15->StackSize, t15->ItemDamage);
            break;
        }
        case P_BLOCKITEMSWITCH:
        {
            BlockItemSwitchPacket *t16;
            t16 = (BlockItemSwitchPacket*)p;
            sprintf(buf, "%c%hd", t16->PacketID, t16->ID);
            break;
        }
        case P_SLEEP:
        {
            SleepPacket *t17;
            t17 = (SleepPacket*)p;
            sprintf(buf, "%c%d%c%d%c%d", t17->PacketID, t17->EntityID, t17->field_22042_e, t17->field_22040_b, t17->field_22044_c, t17->field_22043_d);
            break;
        }
        case P_ARMANIMATION:
        {
            ArmAnimationPacket *t18;
            t18 = (ArmAnimationPacket*)p;
            sprintf(buf, "%c%d%c", t18->PacketID, t18->EntityID, t18->Animate);
            break;
        }
        case P_PACKET19:
        {
            P19Packet *t19;
            t19 = (P19Packet*)p;
            sprintf(buf, "%c%d%c", t19->PacketID, t19->EntityID, t19->State);
            break;
        }
        case P_NAMEDENTITYSPAWN:
        {
            NamedEntitySpawnPacket *t20;
            t20 = (NamedEntitySpawnPacket*)p;
            sprintf(buf, "%c%d%-64s%d%d%d%c%c%hd", t20->PacketID, t20->EntityID, t20->Name, t20->X, t20->Y, t20->Z, t20->Rotation, t20->Pitch, t20->CurrentItem);
            break;
        }
        case P_PICKUPSPAWN:
        {
            PickupSpawnPacket *t21;
            t21 = (PickupSpawnPacket*)p;
            sprintf(buf, "%c%d%hd%c%hd%d%d%d%c%c%c", t21->PacketID, t21->EntityID, t21->ItemID, t21->Count, t21->ItemDamage, t21->X, t21->Y, t21->Z, t21->Rotation, t21->Pitch, t21->Roll);
            break;
        }
        case P_COLLECT:
        {
            CollectPacket *t22;
            t22 = (CollectPacket*)p;
            sprintf(buf, "%c%d%d", t22->PacketID, t22->CollectedEntityID, t22->CollectorEntityID);
            break;
        }
        case P_VEHICLESPAWN:
        {
            VehicleSpawnPacket *t23;
            t23 = (VehicleSpawnPacket*)p;
            sprintf(buf, "%c%d%c%d%d%d", t23->PacketID, t23->EntityID, t23->Type, t23->X, t23->Y, t23->Z);
            break;
        }
        case P_MOBSPAWN:
        {
            MobSpawnPacket *t24;
            t24 = (MobSpawnPacket*)p;
            sprintf(buf, "%c%d%c%d%d%d%c%c", t24->PacketID, t24->EntityID, t24->Type, t24->X, t24->Y, t24->Z, t24->Yaw, t24->Pitch);
            break;
        }
        case P_TITLE:
        {
            TitlePacket *t25;
            t25 = (TitlePacket*)p;
            sprintf(buf, "%c%-64s%d%d%d%d", t25->PacketID, t25->Title, t25->X, t25->Y, t25->Z, t25->Direction);
            break;
        }
        default:
            *buf = '\0';
            return false;
    }
    send(sock, buf, strlen(buf) + 1, 0); // need to do error checking
    return true; // TODO error checking
}


