#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define ROUNDS 6

struct msgbuf {
    long mtype;
    int op;
} buf;

char apt[4][10] = {"error", "剪刀", "石头", "布"};

int main() {
    srand(time(0) ^ getpid());
    key_t key = ftok(".", 1);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid < 0) {
        perror("player1队列创建失败");
        return 1;
    }

    for (int round_num = 1; round_num <= ROUNDS; round_num++) {
        if (msgrcv(msgid, &buf, sizeof(buf)-sizeof(long), 2, 0) < 0) {
            perror("player1接收失败"); exit(1);
        }
        buf.mtype = 1;
        buf.op = rand()%3 + 1;
        printf("回合 %d: 玩家1出招: ", round_num);
        fflush(stdout);
        if (msgsnd(msgid, &buf, sizeof(buf)-sizeof(long), 0) < 0) {
            perror("player1发送失败"); exit(1);
        }
        printf("%s\n", apt[buf.op]);
    }
    return 0;
}
