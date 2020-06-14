/*************************************************************************
	> File Name: YZD.c
	> Author: 
	> Mail: 
	> Created Time: 2020年06月07日 星期日 19时10分33秒
 ************************************************************************/

#include<stdio.h>
#include "../common/udp_epoll.h"
#include "../common/head.h"
#include "../common/udp_server.h"
#include "../common/game.h"

char *conf = "./server.conf";

struct User *rteam;
struct User *bteam;
int data_port;
int port = 0;
typedef struct {
    int sum;
    int *fd;
    int head;
    int tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} TaskQueue;

//struct Map court;
#define MAX_TASK 100
#define MAX_THREAD 20

struct epoll_event ev, events[MAX * 2], B_events[MAX], R_events[MAX];
TaskQueue B_queue, R_queue;

char ch_char(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - 32;
    }
    return c;
}




void do_echo(int fd ) {
    char buf[512] = {0},ch;
    int ind = 0;
    while(1) {
        if(recv(fd,&ch,1,0) <= 0) {
            break;
        } 
        if (ind < sizeof(buf))
        buf[ind++] = ch_char(ch);
        if (ch == '\n') {
            send(fd,buf,ind,0);
            ind = 0;
            continue;
        }
    }
}

void TaskQueueInit (TaskQueue *queue, int sum) {
    queue->sum = sum;
    queue->fd = calloc(sum,sizeof(int));
    queue->head = queue->tail = 0;
    pthread_mutex_init(&queue->mutex,NULL);
    pthread_cond_init(&queue->cond,NULL);
}

void TaskQueuePush(TaskQueue *queue, int fd) {
    pthread_mutex_lock(&queue->mutex);
    queue->fd[queue->tail++] = fd;
    printf(GREEN"<TaskQueuePush>"NONE "%d\n",fd);
    if(queue->tail == queue->sum) {
        queue->tail = 0;
        printf(RED"<end>"NONE"\n");
    } 

    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
}

int TaskQueuePop(TaskQueue *queue) {
    pthread_mutex_lock(&queue->mutex);
    while(queue->tail == queue->head) {
        pthread_cond_wait(&queue->cond,&queue->mutex);
    }
    int fd = queue->fd[queue->head++];

    if(queue->head == queue->sum) {
        queue->head = 0;
    }
    pthread_mutex_unlock(&queue->mutex);
    return fd;
}



void *thread_run(void *arg) {
    pthread_t tid = pthread_self();
    pthread_detach(tid);

    TaskQueue *queue = (TaskQueue *)arg;
    while(1) {
        int fd = TaskQueuePop(queue);
        do_echo(fd);
    }
}

void Pth_create(pthread_t *tid) {
    for (int i = 0; i < MAX; i++) {
         pthread_create(tid,NULL,thread_run,(void *)&R_queue);//工人
     } 
}

int main(int argc, char **argv) {
    int opt, listener, epoll_fd;
    pthread_t draw_t;
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-p port]\n", argv[0]);
                exit(1);
        }
    }
    argc -= (optind - 1);
    argv += (optind - 1);

    if (argc > 1) {
        fprintf(stderr, "Usage: %s [-p port]\n", argv[0]);
        exit(1);
    }

    if (!port) port = atoi(get_value(conf, "PORT"));
    data_port = atoi(get_value(conf, "DATAPORT"));
    court.width = atoi(get_value(conf, "COLS"));
    court.heigth = atoi(get_value(conf, "LINES"));
    court.start.x = 1;
    court.start.y = 1;

    rteam = (struct User *)calloc(MAX, sizeof(struct User));
    bteam = (struct User *)calloc(MAX, sizeof(struct User));

    if ((listener = socket_create_udp(port)) < 0) {
        perror("socket_create_udp");
        exit(1);
    }

    DBG(GREEN"INFO"NONE" : Server start on Port %d\n", port);

    //pthread_create(&draw_t, NULL, draw, NULL);

    epoll_fd = epoll_create(MAX * 2);

    if (epoll_fd < 0) {
        perror("epoll_create");
        exit(1);
    }

    //struct epoll_event ev, events[MAX * 2];
    TaskQueueInit(&B_queue, MAX); //创建两个线程池
    TaskQueueInit(&R_queue, MAX);

    pthread_t *B_tid = calloc(MAX,sizeof(pthread_t));
    pthread_t *R_tid = calloc(MAX,sizeof(pthread_t));
    
    Pth_create(B_tid);// 创建两个线程池
    Pth_create(R_tid);


    ev.events = EPOLLIN;
    ev.data.fd = listener;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listener, &ev);
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    
    int R_ind = 0, B_ind = 0;
    while (1) {
        //w_gotoxy_puts(Message, 1, 1, "Waiting for login");
        //wrefresh(Message);
        DBG(YELLOW"EPOLL"NONE" :  before epoll_wait\n");
        int nfds = epoll_wait(epoll_fd, events, MAX * 2, -1);
        
        DBG(YELLOW"EPOLL"NONE" :  After epoll_wait\n");

        for (int i = 0; i < nfds; i++) {
            char buff[512] = {0};
            DBG(YELLOW"EPOLL"NONE" :  Doing with %dth fd\n", i);
            if (events[i].data.fd == listener) {
                //accept();
                int team = 0;
                udp_accept(epoll_fd, listener,&team);
                if (!team) {
                    R_events[R_ind++].data.fd = listener;
                } else {
                    B_events[B_ind++].data.fd = listener;
                }
            } else {
                recv(events[i].data.fd, buff, sizeof(buff), 0);
                printf(PINK"RECV"NONE" : %s\n", buff);
                //进线程池
                    int B_nfds = epoll_wait(epoll_fd, B_events, MAX, -1);
                    for (int j = 0; j < B_nfds; j++) {
                        TaskQueuePush(&B_queue, B_events[j].data.fd);          
                    }    
                    int R_nfds = epoll_wait(epoll_fd, R_events, MAX, -1);
                    for (int j = 0; j < MAX; j++) {
                        TaskQueuePush(&R_queue, R_events[j].data.fd);
                                                
                    }
                
            }
            //char info[1024] = {0};
            //w_gotoxy_puts(Message, 1, 2, info);
        }

    }
    return 0;
}

