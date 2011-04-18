#ifndef CONNECTION_HPP_INCLUDED
#define CONNECTION_HPP_INCLUDED

/* Connection object encapsulates connecting via
   named pipes or tcp

   Names pipes are used by passing constructor |pipename where
   as with out the leading | will be treated as a tcp hostname

   */

#define _WIN32_WINNT 0x501

#ifdef TARGET_OS_MAC
    #error "TODO sockets lib for osx"
#elif defined __linux__
    #include <sys/types.h>
    #include <sys/socket.h>
#elif defined _WIN32 || defined _WIN64
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #"unknown platform"
#endif

#include <list>

#include "block.hpp"
#include "packet.hpp"

#define RECV_BUF 2048 // TODO put this somewhere better

using namespace std;

typedef enum cs
{
    NEW,
    LISTEN,
    ACCEPTING,
    OPEN,
    CLOSED,
    CONNERROR
} ConnectionState;

class Connection;

typedef bool (*PacketHandler)(Packet *, Connection *);

class Connection
{
    public:
        Connection(); // for handling incoming connections
        Connection(char *port, PacketHandler handler);  //for tcp listening
        //Connection(char *pipename); // connect named pipe
        Connection(char *hostname, int port, PacketHandler handler); // connect tcp
        Connection(SOCKET s, PacketHandler handler); // for accepting connections
        ~Connection();

        Connection *doAccept();
        int  doRouting();
        //void send(const Block *block);
        bool SendPacket(const Packet *packet);
        int doRecv();

        bool isListener();
        ConnectionState getState();
        SOCKET getSocket();
        SOCKET sock;

    private:

        void LogSocketError(void);

        char *_host;
        short int _connectiontype;
        short int _port;

        ConnectionState _connstate;
        sockaddr _clientaddr;
        int _clientaddrlen;



        list<Packet*> inMessages;  //into server
        list<Packet*> outMessages; // out of server

        PacketHandler _handlePacket;

        char _sockbuf[RECV_BUF]; // large enough to hold any packet

};

#endif // CONNECTION_HPP_INCLUDED
