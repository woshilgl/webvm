#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define ROUNDS 6

struct msgbuf {
    long mtype;
    int op;
} buf;

int round_num; // 避免与标准库变量名冲突
char apt[4][10] = {"error", "剪刀", "石头", "布"};

int main() {
    srand(time(0)); // 初始化随机数种子
    key_t key = ftok(".", 1); // 使用与裁判一致的键值
    int msgid;
    if ((msgid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        printf("消息队列 %x 创建失败!\n", key);
        return 1;
    }

    for (round_num = 1; round_num <= ROUNDS; round_num++) {
        // 接收裁判的指令（mtype=2）
        if (msgrcv(msgid, &buf, sizeof(buf) - sizeof(buf.mtype), 2, 0) < 0) {
            perror("接收消息失败");
            return 1;
        }
        if (buf.op == -1) {
            printf("回合 %d: 玩家1出招: ", round_num);
        }

        // 生成随机出招并发送给裁判（mtype=1）
        buf.mtype = 1;
        buf.op = rand() % 3 + 1;
        if (msgsnd(msgid, &buf, sizeof(buf) - sizeof(buf.mtype), 0) < 0) {
            perror("发送消息失败");
            return 1;
        }
        printf("%s\n", apt[buf.op]);
    }
    return 0;
}

