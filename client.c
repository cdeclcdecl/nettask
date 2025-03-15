#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

enum {BUF_SIZE = 1024};

void client(const char *ip, unsigned short port) {
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
        exit(errno);
    }
    snprintf(msg, BUF_SIZE, "server call on %s:%d\n", ip, port);
    if (sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *) &server_addr, size) < 0) {
        dprintf(2, "sending failure\n");
        exit(errno);
    }

    if ((ans_len = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr *) &server_addr, &size)) < 0) {
        dprintf(2, "receiving failure\n");
        exit(errno);
    }

    buf[ans_len] = '\0';
    printf("%s\n", buf);
    shutdown(sockfd, 2);
    close(sockfd);
}