/*************************************************************************
	> File Name: test.c
	> Author: 
	> Mail: 
	> Created Time: 2020年06月11日 星期四 20时08分05秒
 ************************************************************************/

#include "../common/common.h"
#include "../common/head.h"

void *print(void *arg) {
    printf("Hello World\n");
}


int main() {
    pthread_t tid;
    pthread_create(&tid, NULL, print,NULL);
    sleep(10);
    return 0;
}
