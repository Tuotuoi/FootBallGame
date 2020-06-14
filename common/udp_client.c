/*************************************************************************
	> File Name: client.c
	> Author: suyelu
	> Mail: suyelu@haizeix.com
	> Created Time: 日  3/22 20:21:53 2020
 ************************************************************************/

#include  "head.h"

int socket_udp() {
    
    int sockfd;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return -1;
    }
    printf("Socket create.\n");
    return sockfd;

}
