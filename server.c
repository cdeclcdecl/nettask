#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

enum {BUF_SIZE = 1024};

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
        dprintf(2, "Error creating socket\n");
        exit(errno);
    }

    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        dprintf(2, "Error binding socket\n");
        exit(errno);
    }

    while (1) {
        if (recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr *) &client_addr, &size) < 0) {
            dprintf(2, "Error receiving a message\n");
            continue;
        }
        snprintf(ans, BUF_SIZE, "%s:%d", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        if (sendto(sockfd, ans, strlen(ans), 0, (struct sockaddr *) &client_addr, size) < 0) {
            dprintf(2, "Error sending a message\n");
            continue;
        }
    }

    shutdown(sockfd, 2);
    close(sockfd);
}