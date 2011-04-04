
#include "initialize.hpp"

Initialize::Initialize()
{
}

bool Initialize::doInitialization()
{
#ifdef TARGET_OS_MAC
    #error "TODO initialize for osx"
#elif defined __linux__
    return init_linux();
#elif defined _WIN32 || defined _WIN64
    return init_win();
#else
    #error "unknown platform"
#endif
}

Initialize::~Initialize()
{
#ifdef TARGET_OS_MAC
    #error "TODO initialize for osx"
#elif defined __linux__
    cleanup_linux();
#elif defined _WIN32 || defined _WIN64
    cleanup_win();
#else
    #error "unknown platform"
#endif
}

// -----------------

#ifdef TARGET_OS_MAC
    #error "TODO implement init_macosx()"
#elif defined __linux__
bool Initialize::init_linux()
{
    #error "init_linux not implemented"
}

bool Initialize::cleanup_linux()
{
    #error "cleanup_linux not implemented"
}
#elif defined _WIN32 || defined _WIN64
bool Initialize::init_win()
{
    WSADATA wsaData;
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        // TODO: replace with logger
        //printf("WSAStartup failed: %d\n", iResult);
        return false;
    }

    return true;

}
bool Initialize::cleanup_win()
{
    int iResult;

    iResult = WSACleanup();
    if(iResult != 0) {
        // TODO: log failure with logger
        return false;
    }
    return true;
}
#else
    #error "unknown platform"
#endif
