#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

// 1 - 剪刀
// 2 - 石头
// 3 - 布

#define ROUNDS 6

struct msgbuf
{
    long mtype;
    int op;
} buf;

int round_count;
char apt[4][10] = {"error", "剪刀", "石头", "布"};

int main()
{
    srand(time(0)); // 创建随机数种子

    key_t key = ftok(".", 2);
    int msgid;
    if ((msgid = msgget(key, 0666 | IPC_CREAT)) < 0)
    {
        printf("open msg %x failed!\n", key);
        return 0;
    } // 创建消息队列

    for (round_count = 1; round_count < ROUNDS; round_count++)
    {
        memset(&buf, 0, sizeof(buf));
        msgrcv(msgid, &buf, sizeof(buf) - sizeof(buf.mtype), 2, 0);
        if (buf.op == -1)
        {
            printf("round_count %d    请选手2出招:", round_count);
        }
        else
        {
            perror("receive failed!\n");
            return 0;
        }
        memset(&buf, 0, sizeof(buf));
        int op = rand() % 3 + 1;
        buf = (struct msgbuf){1, op};
        msgsnd(msgid, &buf, sizeof(buf) - sizeof(buf.mtype), 0);
        printf("%s\n", apt[op]);
    }
    return 0;
}
