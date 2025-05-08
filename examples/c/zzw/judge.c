#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>

#define ROUNDS 3
#define KEY1 1234
#define KEY2 5678

struct msgbuf {
    long mtype;
    int choice;
};

const char* choices[] = {"无效", "石头", "剪刀", "布"};

int determine_winner(int p1, int p2) {
    if(p1 == p2) return 0;
    if((p1 == 1 && p2 == 2) || 
       (p1 == 2 && p2 == 3) || 
       (p1 == 3 && p2 == 1)) return 1;
    return 2;
}

int main() {
    int msgid1 = msgget(KEY1, 0666 | IPC_CREAT);
    int msgid2 = msgget(KEY2, 0666 | IPC_CREAT);
    struct msgbuf msg;
    int p1_score = 0, p2_score = 0;

    if(msgid1 == -1 || msgid2 == -1) {
        perror("消息队列创建失败");
        return 1;
    }

    for(int round=1; round<=ROUNDS; round++) {
        printf("\n=== 第%d轮 ===\n", round);
        
        // 发送准备信号
        msg.mtype = 1;
        msg.choice = -1;
        msgsnd(msgid1, &msg, sizeof(msg.choice), 0);
        msgsnd(msgid2, &msg, sizeof(msg.choice), 0);

        // 接收玩家选择
        msgrcv(msgid1, &msg, sizeof(msg.choice), 2, 0);
        int p1_choice = msg.choice;
        msgrcv(msgid2, &msg, sizeof(msg.choice), 2, 0);
        int p2_choice = msg.choice;

        // 显示结果
        printf("玩家1出: %s\n", choices[p1_choice]);
        printf("玩家2出: %s\n", choices[p2_choice]);
        
        int result = determine_winner(p1_choice, p2_choice);
        if(result == 0) {
            printf("平局！\n");
        } else {
            printf("玩家%d胜！\n", result);
            result == 1 ? p1_score++ : p2_score++;
        }
    }

    // 最终结果
    printf("\n=== 最终结果 ===\n");
    printf("玩家1得分: %d\n", p1_score);
    printf("玩家2得分: %d\n", p2_score);
    printf("%s\n", p1_score == p2_score ? "平局！" : 
                  p1_score > p2_score ? "玩家1获胜！" : "玩家2获胜！");

    // 清理消息队列
    msgctl(msgid1, IPC_RMID, NULL);
    msgctl(msgid2, IPC_RMID, NULL);
    return 0;
}
