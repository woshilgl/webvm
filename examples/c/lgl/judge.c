/*========== judge.c 最终版 ==========*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

const char* choices[] = {"石头", "剪刀", "布"};

int judge(int p1, int p2) {
    if(p1 == p2) return 0;
    return ((p1 - p2 + 3) % 3 == 1) ? 1 : 2;
}

void print_statistics(int total_rounds, int ai1_wins, int ai2_wins, int draws) {
    printf("\n=== 最终统计 ===\n");
    printf("总对战次数: %d\n", total_rounds);
    printf("玩家1 胜利次数: %d (%.2f%%)\n", 
           ai1_wins, (float)ai1_wins/total_rounds*100);
    printf("玩家2 胜利次数: %d (%.2f%%)\n", 
           ai2_wins, (float)ai2_wins/total_rounds*100);
    printf("平局次数:    %d (%.2f%%)\n", 
           draws, (float)draws/total_rounds*100);
    
    // 判断最终胜利者
    printf("\n=== 最终胜利 ===\n");
    if(ai1_wins > ai2_wins) {
        printf("玩家1 获得最终胜利！\n");
    } else if(ai2_wins > ai1_wins) {
        printf("玩家2 获得最终胜利！\n");
    } else {
        printf("双方势均力敌，平局收场！\n");
    }
}

int main(int argc, char *argv[]) {
    int total_rounds = 10;  // 默认回合数
    
    // 解析命令行参数
    if(argc > 1) {
        int input = atoi(argv[1]);
        if(input > 0) {
            total_rounds = input;
        } else {
            printf("警告：无效回合数，使用默认值 %d\n", total_rounds);
        }
    }

    int ai1_wins = 0, ai2_wins = 0, draws = 0;
    
    printf("=== 自动对战开始 ===\n");
    printf("将进行 %d 轮对战\n\n", total_rounds);

    for(int round = 1; round <= total_rounds; round++) {
        // 创建新的管道
        unlink("/tmp/rps_ai1");
        unlink("/tmp/rps_ai2");
        mkfifo("/tmp/rps_ai1", 0666);
        mkfifo("/tmp/rps_ai2", 0666);

        int fd1 = open("/tmp/rps_ai1", O_RDONLY);
        int fd2 = open("/tmp/rps_ai2", O_RDONLY);

        // 创建玩家进程
        pid_t p1 = fork();
        if(p1 == 0) {
            execl("./player1", "player1", NULL);
            exit(EXIT_FAILURE);
        }
        
        pid_t p2 = fork();
        if(p2 == 0) {
            execl("./player2", "player2", NULL);
            exit(EXIT_FAILURE);
        }

        // 等待玩家完成
        waitpid(p1, NULL, 0);
        waitpid(p2, NULL, 0);

        // 读取选择
        int choice1, choice2;
        read(fd1, &choice1, sizeof(int));
        read(fd2, &choice2, sizeof(int));

        // 判断结果
        int result = judge(choice1, choice2);
        if(result == 1) ai1_wins++;
        else if(result == 2) ai2_wins++;
        else draws++;

        // 显示本轮结果
        printf("第%02d轮: 玩家1=%-6s vs 玩家2=%-6s → ", 
               round, choices[choice1-1], choices[choice2-1]);
        switch(result) {
            case 0: printf("平局\n"); break;
            case 1: printf("玩家1胜\n"); break;
            case 2: printf("玩家2胜\n"); break;
        }

        close(fd1);
        close(fd2);
    }

    print_statistics(total_rounds, ai1_wins, ai2_wins, draws);
    
    // 清理管道
    unlink("/tmp/rps_ai1");
    unlink("/tmp/rps_ai2");
    return 0;
}
