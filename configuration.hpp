#ifndef CONFIGURATION_HPP_INCLUDED
#define CONFIGURATION_HPP_INCLUDED

/* Configuration interface, reads from files
   and serves it out... also writes updates
   to disk
*/
#include <cstdio>

class Configuration
{
    public:
        bool loadFromDisk();
        bool saveToDisk();

        Configuration(char *file);
        ~Configuration();

        char* getValueString();
        short int getValueShort();
        long int getValueLong();

};


#endif // CONFIGURATION_HPP_INCLUDED
