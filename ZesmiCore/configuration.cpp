
#include <cstdio>
#include "configuration.hpp"

Configuration* Configuration::_instance = NULL;

Configuration::Configuration()
{

}

Configuration::~Configuration()
{

}

Configuration* Configuration::getInstance()
{
    if(_instance)
    {
            return _instance;
    }

    _instance = new Configuration();
    return _instance;
}
