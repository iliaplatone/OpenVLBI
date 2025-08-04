#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vlbi.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define SERVER_PORT 5000
#define BACKLOG 5

// Cerca una porta TCP libera (0 = automatico), ritorna porta o -1
int find_free_port() {
    int sockfd;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return -1;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = 0; // porta 0 = sistema sceglie porta libera

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sockfd);
        return -1;
    }

    if (getsockname(sockfd, (struct sockaddr*)&addr, &addrlen) < 0) {
        close(sockfd);
        return -1;
    }

    int port = ntohs(addr.sin_port);
    close(sockfd);
    return port;
}

void handle_client(int client_fd, const char *__file, const char *__arg, ...) {
    int port = find_free_port();
    if (port == -1) {
        const char* err_msg = "Errore nel trovare porta libera\n";
        write(client_fd, err_msg, strlen(err_msg));
        close(client_fd);
        return;
    }

    // Avvia processo figlio che usa la porta
    pid_t pid = fork();
    if (pid == 0) {
        execlp(__file, __arg, NULL);
        perror("execlp");
        _exit(1);
    } else if (pid < 0) {
        perror("fork");
        close(client_fd);
        return;
    }

    // Padre: comunica al client la porta assegnata
    char msg[64];
    snprintf(msg, sizeof(msg), "%d\n", port);
    write(client_fd, msg, strlen(msg));

    close(client_fd);

    // opzionale: puoi fare waitpid non bloccante per pulire i figli zombie
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char **argv) {
    int opt;
    const char *application = NULL;
    const char *options = NULL;
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
    if (application == NULL)
        return -2;
    if(!strcmp(application, "dummy"))
        application = "vlbi_server_dummy";
    else if(!strcmp(application, "indi"))
        application = "vlbi_server_indi";
    else if(!strcmp(application, "json"))
        application = "vlbi_server_json";
    else
        return -2;
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
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

    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        exit(1);
    }

    pwarn("Server listening on port %d\n", port);

    while (1) {
        sin_size = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &sin_size);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        printf("Connection from %s\n", inet_ntoa(client_addr.sin_addr));

        pid_t pid = fork();
        if (pid == 0) {
            close(server_fd);
            handle_client(client_fd, application, NULL);
            exit(0);
        } else if (pid > 0) {
            close(client_fd);
            while(waitpid(-1, NULL, WNOHANG) > 0);
        } else {
            perror("fork");
            close(client_fd);
        }
    }

    close(server_fd);
    return 0;
}
