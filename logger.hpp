
#ifndef LOGGER_HPP_INCLUDED
#define LOGGER_HPP_INCLUDED

#include "connection.hpp"

/* Logs process data to the central registrar and local filesystem */

class Logger
{
    public:
        Logger();  // when logger is on this machine
        Logger(char *loggerhost)  // otherwise logger host
        ~Logger();

    private:
        Connection conn;
}

#endif
