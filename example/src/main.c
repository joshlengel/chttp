#include"chttp/chttp.h"

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

void check(int err)
{
    if (err)
    {
        printf("%s\n", server_get_error_str());
        exit(EXIT_FAILURE);
    }
}

int main(int argc, const char **argv)
{
    if (argc < 2)
    {
        printf("Usage: example.out hostname\n");
        exit(EXIT_FAILURE);
    }

    server_attr_t attr;
    memset(&attr, 0, sizeof(attr));
    attr.hostname = argv[1];
    const char *port_str = getenv("PORT");

    if (!port_str)
    {
        printf("$PORT not specified, using 9090...\n");
        attr.port = 9090;
    }
    else
    {
        attr.port = (uint16_t)strtoul(port_str, NULL, 10);
    }

    server_t *server;
    check(!(server = server_create(&attr)));
    printf("Server up at %s:%hu...\n", attr.hostname, attr.port);
    check(server_listen(server));
    server_destroy(server);
    printf("Server down...\n");
}