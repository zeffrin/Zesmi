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


using namespace std;

typedef enum cs
{
    NEW,
    LISTEN,
    OPEN,
    CLOSED,
    CONNERROR
} ConnectionState;

class Connection
{
    public:
        Connection(char *port);  //for tcp listening
        //Connection(char *pipename); // connect named pipe
        Connection(char *hostname, char *port); // connect tcp
        Connection(SOCKET s); // for accepting connections
        ~Connection();

        Connection *doAccept();
        //void send(const Block *block);
        void SendPacket(const Packet *packet, PacketType pt);
        bool doRecv();

        bool isListener();
        SOCKET getSocket();
        SOCKET sock;

    private:
        char *_host;
        short int _connectiontype;
        short int _port;

        ConnectionState _connstate;
        sockaddr _clientaddr;
        int _clientaddrlen;



        list<Packet*> inMessages;  //into server
        list<Packet*> outMessages; // out of server

        char _sockbuf[1092]; // large enough to hold any packet

};

#endif // CONNECTION_HPP_INCLUDED
