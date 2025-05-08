#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <time.h>

#define KEY2 5678

struct msgbuf {
    long mtype;
    int choice;
};

int main() {
    srand(time(NULL));
    int msgid = msgget(KEY2, 0666);
    struct msgbuf msg;

    while(1) {
        // 等待裁判信号
        msgrcv(msgid, &msg, sizeof(msg.choice), 1, 0);
        
        if(msg.choice == -1) {
            // 生成随机选择
            msg.mtype = 2;
            msg.choice = rand() % 3 + 1;
            msgsnd(msgid, &msg, sizeof(msg.choice), 0);
        }
    }
    return 0;
}
