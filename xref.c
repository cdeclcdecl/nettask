#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    pid_t pid;
    int port;
    char arg[10];
    printf("enter port number: ");
    scanf("%d", &port);
    sprintf(arg, "%d", port);
    if (!(pid = fork())) {
        execl("/home/cdecl/2ip", "2ip", "-s", "192.168.10.11", arg, 0);
	printf("problem\n");
        exit(0);
    }
    printf("process with pid: %d is running\nto connect use 192.168.10.11:%d\n", pid, port);
 }
