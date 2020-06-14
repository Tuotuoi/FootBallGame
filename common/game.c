/*************************************************************************
	> File Name: game.h
	> Author: 
	> Mail: 
	> Created Time: 2020年06月02日 星期二 18时35分46秒
 ************************************************************************/
#ifndef _GAME_H
#define _GAME_H

#define MAX 50
#include "../common/head.h"
#include "../common/common.h"
#include "../common/datatype.h"
#include "./game.h"
extern struct Map court;
extern WINDOW *Football, *Message, *Help, *Score, *Write;
extern WINDOW *create_newwin(int width, int heigth, int startx, int starty);
int message_num = 0;
WINDOW *create_newwin(int width, int heigth, int startx, int starty) {
            //创建一个窗口
    WINDOW *win;
    win = newwin(heigth, width, starty, startx); // nurses库 [y,x]反者来的
    box(win, 0, 0); // 从【0，0】加
    wrefresh(win);
    return win;
}

void destroy_win(WINDOW *win) {
    wborder(win, ' ', ' ',' ', ' ', ' ', ' ',' ', ' '); //8ge 空格删除线
    wrefresh(win);
    delwin(win);
}

void gotoxy(int x, int y) {
    move(y, x);
}

void gotoxy_putc(int x, int y, int c) {
    move(y, x);
    addch(c);
    move(LINES - 1, 1);         //将光标移走，不影响美观
    refresh();
}
void gotoxy_puts(int x, int y, char *s) {
    move(y, x);
    addstr(s);
    move(LINES - 1, 1);
    refresh();
}

void w_gotoxy_putc(WINDOW *win, int x, int y, int c) {
    mvwaddch(win, y, x, c);
    move(LINES - 1, 1);
    wrefresh(win);
    return ;
}
void w_gotoxy_puts(WINDOW *win, int x, int y, char *s) {
    mvwprintw(win, y, x, s);
    move(LINES - 1, 1);
    wrefresh(win);
    return ;
}

void initfootball() {
    initscr();
    clear();
    if (!has_colors() || start_color() == ERR) {
        endwin();
        fprintf(stderr, "终端不支持颜色!\n");
        return ;
    } 
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_BLUE, COLOR_BLACK);

    Football = create_newwin(court.width, court.heigth, court.start.x, court.start.y) ;  //绘画一个球场边框              
    wrefresh(Football);
    WINDOW *Message_t = create_newwin(court.width, 7, court.start.x, court.start.y + court.heigth) ;                
    Message = subwin(Message_t, 5, court.width - 2, court.start.y + court.heigth + 1, court.start.x + 1);
    scrollok(Message, 1);
    wrefresh(Message);
    Help = create_newwin(20, court.heigth, court.start.x + court.width, court.start.y) ;                
    wrefresh(Help);
    Score = create_newwin(20, 7, court.start.x + court.width, court.start.y + court.heigth);
    wrefresh(Score);
    Write = create_newwin(20 + court.width, 7, court.start.x , court.start.y + court.heigth + 7);
    wrefresh(Write);

    move(LINES - 1, 1);

}
void *draw(void *arg) {
    initfootball();
    while(1) {
        sleep(10);
    }
}


void show_message(WINDOW *win, struct User *user, char *msg, int type) {
    time_t time_now = time(NULL);
    struct tm* tm = localtime(&time_now);
    char timestr[20] = {0};
    char username[80] = {0};
    sprintf(timestr,"%02d:%02d:%02d", tm->tm_hour,tm->tm_min, tm->tm_sec);
    if (type == 1) {
        wattron(win,COLOR_PAIR(4));
        strcpy(username,"Server info :");
    } else {
        if (user->team) {
            wattron(win, COLOR_PAIR(6));
        } else {
            wattron(win, COLOR_PAIR(2));
        }
        sprintf(username, "%s :",user->name);
    }
    if (message_num < 4) {
        w_gotoxy_puts(win, 10, message_num, username);
        wattron(win,COLOR_PAIR(3));
        w_gotoxy_puts(win, 10 + strlen(username), message_num, msg);
        wattron(win,COLOR_PAIR(5));
        w_gotoxy_puts(win, 1, message_num, timestr);
        message_num++;
    } else {
        message_num = 4;
        scroll(win);
        w_gotoxy_puts(win, 10, message_num, username);
        wattron(win,COLOR_PAIR(3));
        w_gotoxy_puts(win, 10 + strlen(username), message_num, msg);
        wattron(win,COLOR_PAIR(5));
        w_gotoxy_puts(win, 1, message_num, timestr);
        message_num++;
    }
    wrefresh(win);
}

#endif

