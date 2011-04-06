
#include "logger.hpp"

#define LOGFILE "server.log" //TODO move to central config

Logger* Logger::_instance = NULL;

Logger::Logger()
{
    _conn = NULL;
    _logfile = fopen(LOGFILE, "w");
    if(_logfile)
    {
        writeToLog("Logging to file: " LOGFILE);
    }
    else
    {
        writeToLog("LOGGER: ERROR unable to open " LOGFILE " for writing.\n");
    }
}

Logger::~Logger()
{
    fclose(_logfile);
}

bool Logger::connect(Connection *connection)
{

    if(_conn) {
        writeToLog("LOGGER: Logger::Initialize need only be called once\n");
        return false;
    }
    if(!connection)
    {
        writeToLog("LOGGER: Logger:Initialize passed null value\n");
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
    if(_logfile)
    {
        fprintf(_logfile, "%s", msg);
    }
    return;
}
