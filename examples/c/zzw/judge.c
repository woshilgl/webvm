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
} buf1, buf2;

int player_1[ROUNDS], player_2[ROUNDS], winner[ROUNDS]; // 选手操作    胜者
int round_count;                                              // 回合数
int count[3];                                           // 0 - 平局/ 1,2 - 选手 获胜次数
char apt[4][10] = {"error", "剪刀", "石头", "布  "};

int max(int a, int b)
{
    return a > b ? a : b;
}

int judge()
{

    // 0 - 平局
    // 1 - player1 WIN!
    // 2 - player2 WIN!

    int op1 = player_1[round_count], op2 = player_2[round_count];
    if (op1 == op2)
        return 0;
    if (op1 + op2 != 4)
        return op1 > op2 ? 1 : 2;
    return op1 < op2 ? 1 : 2;
}

int main()
{
    int i;
    key_t key_1 = ftok(".", 1);
    key_t key_2 = ftok(".", 2);
    int msgid_1 = msgget(key_1, 0666 | IPC_CREAT);
    int msgid_2 = msgget(key_2, 0666 | IPC_CREAT);

    for (round_count = 1; round_count < ROUNDS; round_count++)
    {
        printf("ROUND %d:\n", round_count);
        printf("请两位选手出招！\n");
        memset(&buf1, 0, sizeof(buf1));
        memset(&buf2, 0, sizeof(buf2));
        buf1 = (struct msgbuf){2, -1};
        buf2 = (struct msgbuf){2, -1};
        msgsnd(msgid_1, &buf1, sizeof(buf1) - sizeof(buf1.mtype), 0);
        msgsnd(msgid_2, &buf2, sizeof(buf2) - sizeof(buf2.mtype), 0);
        memset(&buf1, 0, sizeof(buf1));
        memset(&buf2, 0, sizeof(buf2));
        msgrcv(msgid_1, &buf1, sizeof(buf1) - sizeof(buf1.mtype), 1, 0);
        msgrcv(msgid_2, &buf2, sizeof(buf2) - sizeof(buf2.mtype), 1, 0);

        printf("选手1出:%s\n", apt[buf1.op]);
        printf("选手2出:%s\n", apt[buf2.op]);

        player_1[round_count] = buf1.op;
        player_2[round_count] = buf2.op;
        int win = judge();
        winner[round_count] = win;
        count[win]++;
        if (win == 0)
            printf("平局！\n\n");
        else
            printf("选手%d胜!\n\n", win);
    }
    printf("选手1的依次出招为:");
    for ( i = 1; i < ROUNDS; i++)
    {
        printf("%s, ", apt[player_1[i]]);
    printf("\n");
    printf("选手2的依次出招为:");
    }
    for ( i = 1; i < ROUNDS; i++){
        printf("%s, ", apt[player_2[i]]);
    printf("\n");
    printf("平局次数为%d\n", count[0]);
    printf("选手1获胜的回合数为%d\n", count[1]);
    printf("选手2获胜的回合数为%d\n", count[2]);
    }
    if (count[1] == count[2])
        printf("此次比赛，两位选手平局！\n");
    if (count[1] > count[2])
        printf("此次比赛,选手1胜利!\n");
    if (count[1] < count[2])
        printf("此次比赛,选手2胜利!\n");
}
