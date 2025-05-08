#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <time.h>

struct msgbuf {
    long mtype;
    int choice;
};

int main() {
    srand(time(NULL));
    key_t key = ftok("/tmp", 'B'); // player2.c中改为'B'
    int msgid = msgget(key, 0666);
    struct msgbuf msg;

    if(msgid == -1) {
        perror("消息队列连接失败");
        return 1;
    }

    while(1) {
        msgrcv(msgid, &msg, sizeof(msg.choice), 1, 0);
        if(msg.choice == -1) {
            msg.mtype = 2;
            msg.choice = rand() % 3 + 1;
            msgsnd(msgid, &msg, sizeof(msg.choice), 0);
        }
    }
    return 0;
}
