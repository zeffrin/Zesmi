#ifndef INITIALIZE_HPP_INCLUDED
#define INITIALIZE_HPP_INCLUDED

/*  initialize everything required for a node
    connection, logger, etc
*/

#ifdef TARGET_OS_MAC
    #error "TODO sockets lib for osx"
#elif defined __linux__
    #include <sys/types.h>
    #include <sys/socket.h>
#elif defined _WIN32 || defined _WIN64
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #error "unknown platform"
#endif

using namespace std;

class Initialize
{

    public:
        Initialize();
        ~Initialize();
        bool doInitialization();

    private:
        bool init_win();
        bool init_linux();
        bool init_macosx();

        bool cleanup_win();
        bool cleanup_linux();
        bool cleanup_macosx();

};

#endif // INITIALIZE_HPP_INCLUDED
