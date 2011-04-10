
#ifndef LOGGER_HPP_INCLUDED
#define LOGGER_HPP_INCLUDED

#include <stdio.h>

#include "connectioncontroller.hpp"

/* Logs process data to the central registrar and local filesystem */
/* Use getInstance() to get ptr to singleton and connect() to provide */
/* connection to other host */

class Logger
{
    public:

       static Logger* getInstance();

        void writeToLog(char *msg);
         bool connect(Connection *connection);
        ~Logger();  // otherwise logger host


    private:
        Connection *_conn;
        Logger();  // when logger is on this machine

        static Logger *_instance;
        FILE *_logfile;
};

#endif
