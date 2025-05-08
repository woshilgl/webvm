#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

int main() {
    srand(time(NULL) ^ getpid()); // 增强随机性
    int choice = rand() % 3 + 1;  // 生成随机选择

    int fd = open("/tmp/rps_ai1", O_WRONLY);
    write(fd, &choice, sizeof(int));
    close(fd);
    return 0;
}
