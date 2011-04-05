
#include "logger.hpp"
#include "stdio.h"

Logger* Logger::_instance = NULL;

Logger::Logger()
{
    _conn = NULL;
}

Logger::~Logger()
{

}

bool Logger::connect(Connection *connection)
{

    if(_conn) {
        printf("LOGGER: Logger::Initialize need only be called once\n");
        return false;
    }
    if(!connection)
    {
        printf("LOGGER: Logger:Initialize passed null value\n");
        return false;
    }
    _conn = connection;
    return true;

}

Logger* Logger::getInstance()
{
    if(_instance)
      return _instance;
    _instance = new Logger();
    return _instance;
}

void Logger::writeToLog(char *msg)
{
    printf("%s", msg);

    if(_conn)
    {
        // send packet to logger
    }
    return;
}
