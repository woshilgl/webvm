#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>

#define ROUNDS 6

struct msgbuf {
    long mtype;
    int op;
} buf1, buf2;

int player_1[ROUNDS + 1], player_2[ROUNDS + 1], winner[ROUNDS + 1];
int round_num;
int count[3] = {0}; // [0]平局，[1]玩家1胜，[2]玩家2胜
char apt[4][10] = {"error", "剪刀", "石头", "布"};

int judge(int op1, int op2) {
    if (op1 == op2) return 0;
    if ((op1 - op2 + 3) % 3 == 1) return 1;
    else return 2;
}

int main() {
    key_t key1 = ftok(".", 1); // 玩家1的消息队列键值
    key_t key2 = ftok(".", 2); // 玩家2的消息队列键值
    int msgid1 = msgget(key1, 0666 | IPC_CREAT);
    int msgid2 = msgget(key2, 0666 | IPC_CREAT);

    if (msgid1 < 0 || msgid2 < 0) {
        perror("消息队列创建失败");
        return 1;
    }

    for (round_num = 1; round_num <= ROUNDS; round_num++) {
        printf("===== 回合 %d =====\n", round_num);

        // 发送出招指令（mtype=2，op=-1）
        buf1.mtype = 2;
        buf1.op = -1;
        buf2.mtype = 2;
        buf2.op = -1;
        msgsnd(msgid1, &buf1, sizeof(buf1) - sizeof(buf1.mtype), 0);
        msgsnd(msgid2, &buf2, sizeof(buf2) - sizeof(buf2.mtype), 0);

        // 接收玩家出招（mtype=1）
        msgrcv(msgid1, &buf1, sizeof(buf1) - sizeof(buf1.mtype), 1, 0);
        msgrcv(msgid2, &buf2, sizeof(buf2) - sizeof(buf2.mtype), 1, 0);

        player_1[round_num] = buf1.op;
        player_2[round_num] = buf2.op;
        int win = judge(buf1.op, buf2.op);
        winner[round_num] = win;
        count[win]++;

        printf("玩家1出招: %s\n", apt[buf1.op]);
        printf("玩家2出招: %s\n", apt[buf2.op]);
        if (win == 0) printf("平局！\n\n");
        else printf("玩家%d胜利！\n\n", win);
    }

    // 统计最终结果
    printf("===== 最终统计 =====\n");
    printf("玩家1的出招序列: ");
    for (int i = 1; i <= ROUNDS; i++) printf("%s ", apt[player_1[i]]);
    printf("\n玩家2的出招序列: ");
    for (int i = 1; i <= ROUNDS; i++) printf("%s ", apt[player_2[i]]);
    printf("\n平局次数: %d\n", count[0]);
    printf("玩家1胜利次数: %d\n", count[1]);
    printf("玩家2胜利次数: %d\n", count[2]);

    if (count[1] == count[2]) printf("比赛结果为平局！\n");
    else printf("最终胜利者: 玩家%d\n", count[1] > count[2] ? 1 : 2);

    // 清理消息队列
    msgctl(msgid1, IPC_RMID, NULL);
    msgctl(msgid2, IPC_RMID, NULL);
    return 0;
}
