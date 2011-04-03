#ifndef PLAYER_HPP_INCLUDED
#define PLAYER_HPP_INCLUDED

#include "living.hpp"
#include "connection.hpp"

class Player : public Living
{
    public:
        Player();
        ~Player();

    private:
        Connection *_conn;
        char *_hostname;
        char *_ipaddress;
        char *_name;
};
#endif // PLAYER_H_INCLUDED
