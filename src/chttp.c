#include"chttp/chttp.h"

#include<sys/socket.h>
#include<netdb.h>
#include<unistd.h>

#include<assert.h>
#include<errno.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>

#include<pthread.h>

#include"vector.h"

struct _server_t
{
    int socket;
    int backlog;
};

// Error messages
#define SERVER_ERR_LEN 200

static char server_err[SERVER_ERR_LEN + 1] = { '\0' };
const char *server_get_error_str() { return server_err; }

// Signal handling
#define MAX_OPEN_SERVERS 10
#define WRITE_MSG(msg) { int e = write(STDOUT_FILENO, msg, sizeof(msg)); (void)e; }

static int shutdown_pending;

static void handle_signal(int sig);

__attribute__((constructor)) static void setup_global()
{
    shutdown_pending = 0;

    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_flags = 0;
    action.sa_handler = handle_signal;

    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);
}

void handle_signal(int sig)
{
    (void)sig;
    if (shutdown_pending) exit(130);

    WRITE_MSG("SIGTERM or SIGINT received, attempting graceful shutdown of all open servers...\n");

    shutdown_pending = 1;
}

// Server
void _set_server_err(const char *func, const char *err)
{
    size_t off = 0;
    memcpy(server_err + off, func, strlen(func)); off += strlen(func);
    memcpy(server_err + off, ": ", strlen(": ")); off += strlen(": ");
    memcpy(server_err + off, err, strlen(err)); off += strlen(err);
    server_err[off] = '\0';
}

server_t *server_create(const server_attr_t *attr)
{
    int res;

    static const server_attr_t DEFAULT_ATTR = { .hostname="localhost", .port=9090, .backlog=100000 };
    if (!attr) attr = &DEFAULT_ATTR;

    const char *hostname = attr->hostname? attr->hostname : DEFAULT_ATTR.hostname;
    uint16_t port = attr->port? attr->port : DEFAULT_ATTR.port;
    int backlog = attr->backlog? attr->backlog : DEFAULT_ATTR.backlog;

    char port_str[6];
    if (snprintf(port_str, sizeof(port_str), "%hu", port) < 0)
    {
        _set_server_err("server_create", strerror(errno));
        return NULL;
    }

    // Create socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        _set_server_err("server_create", strerror(errno));
        return NULL;
    }

    int reuse_addr = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)) < 0)
    {
        _set_server_err("server_create", strerror(errno));
        close(fd);
        return NULL;
    }

    // Get specified address
    struct addrinfo hints, *addr;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = 0;

    if ((res = getaddrinfo(hostname, port_str, &hints, &addr)) < 0)
    {
        _set_server_err("server_create", gai_strerror(res));
        close(fd);
        return NULL;
    }

    // Bind server socket to address
    res = bind(fd, addr->ai_addr, addr->ai_addrlen);
    freeaddrinfo(addr);
    if (res < 0)
    {
        _set_server_err("server_create", strerror(errno));
        close(fd);
        return NULL;
    }

    // Create server struct
    server_t *server = malloc(sizeof(server_t));
    if (!server)
    {
        _set_server_err("server_create", "No memory");
        close(fd);
        return NULL;
    }

    server->socket = fd;
    server->backlog = backlog;

    return server;
}

int server_listen(server_t *server)
{
    assert(server != NULL);

    if (listen(server->socket, server->backlog) < 0)
    {
        _set_server_err("server_listen", strerror(errno));
        return -1;
    }

    // Enter loop
    while (!shutdown_pending)
    {
        int client_socket = accept(server->socket, NULL, NULL);
        if (client_socket < 0)
        {
            switch (errno)
            {
                case EINTR: return 0;
                case ENETDOWN:
                case EPROTO:
                case ENOPROTOOPT:
                case EHOSTDOWN:
                case EHOSTUNREACH:
                case EOPNOTSUPP:
                case ENETUNREACH: continue;
                default: break;
            }

            _set_server_err("server_listen", strerror(errno));
            return -1;
        }

        printf("Client connected!\n");

        if (shutdown(client_socket, SHUT_RDWR) || close(client_socket))
        {
            _set_server_err("server_listen", strerror(errno));
            return -1;
        }
    }

    return 0;
}

void server_destroy(server_t *server)
{
    assert(server != NULL);

    (void)(close(server->socket));

    free(server);
}