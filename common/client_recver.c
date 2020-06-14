/*************************************************************************
	> File Name: client_recver.c
	> Author: 
	> Mail: 
	> Created Time: 2020年06月13日 星期六 14时13分33秒
 ************************************************************************/

#include "head.h"
#include "common.h"
#include "datatype.h"
#include "../common/game.h"
extern int sockfd;
void *client_recv(void *arg) {
    while (1) {
        struct FootBallMsg msg;
        struct User user;
        bzero(&msg, sizeof(msg));
        recv(sockfd, (void *)&msg,sizeof(msg),0);
        strcpy(user.name, msg.name);
        user.team = msg.team;
        if (msg.type & FT_TEST) {
            DBG(RED"HeartBeat from Server 心跳"NONE"\n");
            msg.type = FT_ACK;
            send(sockfd, (void *)&msg, sizeof(msg), 0);
            
        } else if (msg.type & FT_MSG) {
            DBG(GREEN"Server Msg : "NONE" %s \n", msg.msg);
            Show_Message(,&user,msg.msg, 0);
            
        } else if (msg.type & FT_WALL) {
            Show_Message(,NULL,msg.msg, 1);
            
        } else if (msg.type & FT_FIN) {
            DBG(GREEN"Server is going to stop."NONE"\n");
            endwin();
            exit(0);
        } else {
            DBG(GREEN"Msg Unspport!"NONE"\n");
        }
    }
}

