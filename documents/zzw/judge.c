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
int count[3] = {0};
char apt[4][10] = {"error", "剪刀", "石头", "布"};

int judge(int op1, int op2) {
    if (op1 == op2) return 0;
    return (op1 - op2 + 3) % 3 == 1 ? 1 : 2;
}

void cleanup_queues(key_t key1, key_t key2) {
    int temp_msgid;
    if ((temp_msgid = msgget(key1, 0666)) >= 0) {
        msgctl(temp_msgid, IPC_RMID, NULL);
    }
    if ((temp_msgid = msgget(key2, 0666)) >= 0) {
        msgctl(temp_msgid, IPC_RMID, NULL);
    }
}

int main() {
    key_t key1 = ftok(".", 1);
    key_t key2 = ftok(".", 2);
    cleanup_queues(key1, key2); // 清理旧队列

    int msgid1 = msgget(key1, 0666 | IPC_CREAT);
    int msgid2 = msgget(key2, 0666 | IPC_CREAT);
    if (msgid1 < 0 || msgid2 < 0) {
        perror("消息队列创建失败");
        return 1;
    }

    for (int round_num = 1; round_num <= ROUNDS; round_num++) {
        printf("===== 回合 %d =====\n", round_num);

        // 发送出招指令
        buf1.mtype = 2; buf1.op = -1;
        buf2.mtype = 2; buf2.op = -1;
        if (msgsnd(msgid1, &buf1, sizeof(buf1)-sizeof(long), 0) < 0) {
            perror("发送给玩家1失败"); exit(1);
        }
        if (msgsnd(msgid2, &buf2, sizeof(buf2)-sizeof(long), 0) < 0) {
            perror("发送给玩家2失败"); exit(1);
        }

        // 接收出招
        if (msgrcv(msgid1, &buf1, sizeof(buf1)-sizeof(long), 1, 0) < 0) {
            perror("接收玩家1失败"); exit(1);
        }
        if (msgrcv(msgid2, &buf2, sizeof(buf2)-sizeof(long), 1, 0) < 0) {
            perror("接收玩家2失败"); exit(1);
        }

        player_1[round_num] = buf1.op;
        player_2[round_num] = buf2.op;
        int win = judge(buf1.op, buf2.op);
        count[win]++;
        
        printf("玩家1出招: %s\n", apt[buf1.op]);
        printf("玩家2出招: %s\n", apt[buf2.op]);
        printf(win ? "玩家%d胜利！\n\n" : "平局！\n\n", win);
    }

    printf("===== 最终统计 =====\n");
    printf("玩家1序列: ");
    for (int i = 1; i <= ROUNDS; i++) printf("%s ", apt[player_1[i]]);
    printf("\n玩家2序列: ");
    for (int i = 1; i <= ROUNDS; i++) printf("%s ", apt[player_2[i]]);
    printf("\n平局: %d\n玩家1胜: %d\n玩家2胜: %d\n", count[0], count[1], count[2]);
    printf("最终胜利者: %s\n", count[1]==count[2] ? "平局" : count[1]>count[2] ? "玩家1" : "玩家2");

    msgctl(msgid1, IPC_RMID, NULL);
    msgctl(msgid2, IPC_RMID, NULL);
    return 0;
}
