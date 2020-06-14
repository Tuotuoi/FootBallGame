/*************************************************************************
	> File Name: ../common/udp_epoll.h
	> Author: 
	> Mail: 
	> Created Time: Thu 04 Jun 2020 07:24:00 PM CST
 ************************************************************************/

#ifndef _UDP_EPOLL_H
#define _UDP_EPOLL_H
#include "datatype.h"
void add_event(int epollfd, int fd, int events);
void del_event(int epollfd, int fd);
//int udp_connect(int epollfd, struct sockaddr_in *serveraddr);
int udp_accept(int epollfd, int fd, struct User *user);
void add_to_sub_reactor(struct User *user);
#endif
