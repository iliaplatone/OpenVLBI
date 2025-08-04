#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vlbi.h>
#include <vlbi_server.h>
#include <vlbi_server_indi.h>
#include <vlbi_server_dummy.h>
#include <vlbi_server_json.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define VLBI_LIBRARY
#define SERVER_PORT 5000
#define BACKLOG 5

Server* vlbi_server;

int main(int argc, char **argv) {
    int opt;
    const char *application = nullptr;
    const char *options = nullptr;
    int port = SERVER_PORT;
    while ((opt = getopt(argc, argv, "s:o:p:h")) != -1)
    {
        switch (opt)
        {
            case 's':
                application = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            default:
                perr("Usage: %s [-s [dummy|indi|json]] [-p listen_port]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if (application == nullptr)
        return -2;
    if(!strcmp(application, "dummy"))
        vlbi_server = new DUMMYServer();
    else if(!strcmp(application, "indi"))
        vlbi_server = new INDIServer();
    else if(!strcmp(application, "json"))
        vlbi_server = new JSONServer();
    else
        return -2;
    int server_fd;
    struct sockaddr_in server_addr;
    socklen_t sin_size;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(1);
    }

    int optval = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(server_fd, 0) < 0) {
        perror("listen");
        exit(1);
    }

    pwarn("Server listening on port %d\n", port);
    sin_size = sizeof(client_addr);

    while (1) {
        struct sockaddr* client_addr;
        memset(client_addr, 0, sin_size);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &sin_size);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        printf("Connection from %s\n", inet_ntoa(client_addr.sin_addr));

        dsp_set_debug_level(40);
        server->setInput(fdopen(client_fd, "rw"));
        server->setOutput(fdopen(client_fd, "rw"));

        server->Init(0, nullptr);
        while(client_fd > 0)
        {
            if(feof(server->getInput()))
                break;
            server->Parse();
        }
        server->~Server();
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
