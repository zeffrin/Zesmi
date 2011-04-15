#include "logger.hpp"

#define LOGFILE "server.log" //TODO move to central config

Logger* Logger::_instance = NULL;

Logger::Logger()
{
    _conn = NULL;
    _logfile = fopen(LOGFILE, "w");
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

/*
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

*/

void Logger::writeToLog(char *fmt, ...)
{
    va_list ap;
    void *p;
    int i = 0;
    void *ptrs[6];

    va_start(ap, fmt);
    while( (i < 6 + 1) && ( *(p = va_arg(ap, void*) ) != NULL))  // I guess void * here was wishful thinking
    {
        ptrs[i] = p;
        i++;
    }

    //ugly cuz I dont know a better way
    char buf [1024];
    switch(i)
    {
        case 1:     sprintf(&buf, fmt, ptrs[0]);
                    break;
        case 2:     sprintf(&buf, fmt, ptrs[0], ptrs[1]);
                    break;
        case 3:     sprintf(&buf, fmt, ptrs[0], ptrs[1], ptrs[2]);
                    break;
        case 3:     sprintf(&buf, fmt, ptrs[0], ptrs[1], ptrs[2], ptrs[3]);
                    break;
        case 4:     sprintf(&buf, fmt, ptrs[0], ptrs[1], ptrs[2], ptrs[3], ptrs[4]);
                    break;
        case 5:     sprintf(&buf, fmt, ptrs[0], ptrs[1], ptrs[2], ptrs[3], ptrs[4], ptrs[5]);
                    break;
        case 6:     sprintf(&buf, fmt, ptrs[0], ptrs[1], ptrs[2], ptrs[3], ptrs[4], ptrs[5], ptrs[6]);
                    break;

        default:    sprintf(&buf, fmt);
    }
    writeToLog(buf);

}
