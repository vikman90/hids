/* July 23, 2019 */

#ifndef AGENT_H
#define AGENT_H

#include "fim.h"
#include "logcollector.h"

typedef struct {
    char * address;
    unsigned short port;
} server_t;

typedef struct {
    server_t * servers;
    unsigned servers_length;
} client_t;

extern client_t client;

#endif
