#pragma once

#include<stdint.h>
#include<stddef.h>

typedef struct _server_t server_t;

typedef struct
{
    const char *hostname;
    uint16_t port;
    int backlog;
} server_attr_t;

const char *server_get_error_str();

server_t *server_create(const server_attr_t *attr);
int server_listen(server_t *server);
void server_destroy(server_t *server);