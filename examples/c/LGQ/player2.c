#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

int main() {
    srand(time(NULL) ^ getpid()); // 不同的随机种子
    int choice = rand() % 3 + 1;

    int fd = open("/tmp/rps_ai2", O_WRONLY);
    write(fd, &choice, sizeof(int));
    close(fd);
    return 0;
}
