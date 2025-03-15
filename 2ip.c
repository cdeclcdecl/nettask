#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

enum {
    BUF_SIZE = 1024,
    CLIENT_MODE = false,
    SERVER_MODE = true,
    TIMELIMIT = 5
};

void timeout(int sig) {
    printf("server is not responding\n");
    fflush(stdout);
}

void terminate(int sockfd) {
    shutdown(sockfd, 2);
    close(sockfd);
    exit(errno);
}

void client(const char *ip, unsigned short port) {
    signal(SIGALRM, timeout);
    alarm(TIMELIMIT);
    int sockfd;
    struct sockaddr_in server_addr;
    char buf[BUF_SIZE], msg[BUF_SIZE];
    unsigned size = sizeof(server_addr);
    int ans_len;
    memset(&server_addr, 0, size);
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        dprintf(2, "socket creation failure\n");
        terminate(sockfd);
    }
    snprintf(msg, BUF_SIZE, "server call on %s:%d\n", ip, port);
    if (sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *) &server_addr, size) < 0) {
        dprintf(2, "sending failure\n");
        terminate(sockfd);
    }

    if ((ans_len = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr *) &server_addr, &size)) < 0) {
        dprintf(2, "receiving failure\n");
        terminate(sockfd);
    }

    buf[ans_len] = '\0';
    printf("%s\n", buf);
    shutdown(sockfd, 2);
    close(sockfd);
}


void server(const char *ip, unsigned short port) {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buf[BUF_SIZE], ans[BUF_SIZE];
    unsigned size = sizeof(client_addr);
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, size);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        dprintf(2, "Error: creating socket failure\n");
        exit(errno);
    }

    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        dprintf(2, "Error: socket binding failure\n");
        terminate(sockfd);
    }

    while (1) {
        if (recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr *) &client_addr, &size) < 0) {
            dprintf(2, "Error: cannot receive message (invalid parameters)\n");
            continue;
        }
        snprintf(ans, BUF_SIZE, "%s:%d", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        if (sendto(sockfd, ans, strlen(ans), 0, (struct sockaddr *) &client_addr, size) < 0) {
            dprintf(2, "Error: cannot send message (invalid parameters)\n");
            continue;
        }
    }

    shutdown(sockfd, 2);
    close(sockfd);
}

const char *help_msg = "usage: ./2ip <flags> <ip> <port>\navailible flags:\n-s (--server) - run the program -n server mode\n-h (--help) - show help message";

int main(int argc, char **argv) {
    if (argc < 3) {
        dprintf(2, "%s", help_msg);
        exit(0);
    }
    bool mode = CLIENT_MODE;
    char buf[BUF_SIZE];
    for (int i = 1; i < argc - 2; i++) {
        sscanf(argv[i], "%s", buf);
        if (!strcmp(buf, "-s") || !strcmp(buf, "--server")) {
            mode = SERVER_MODE;
        } else if (!strcmp(buf, "-h") || !strcmp(buf, "--help")) {
            printf("%s", help_msg);
        } 
        else {
            exit(1);
        }
    }
    char ip[BUF_SIZE];
    unsigned short port;
    sscanf(argv[argc - 2], "%s", ip);
    sscanf(argv[argc - 1], "%hu", &port);
    if (mode == SERVER_MODE) {
        server(ip, port);
    } else {
        client(ip, port);
    }
}