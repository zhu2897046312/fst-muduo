#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_EVENTS 5
/**
 *  epoll使用示例
 *      1. 创建epoll_fd
 *      2. 注册监听事件
 *      3. epoll_wait
 *          （1） fd 触发事件 --- 从epoll_wait()出来
 *           (2) fd 未触发事件 但epoll_wait 等待超时 --- 从epoll_wait()出来 重复该操作
 */
int main() {
    int epoll_fd = epoll_create1(0);    
    if (epoll_fd == -1) {
        perror("epoll_create1");
        return 1;
    }

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET; // 设置为边缘触发模式
    event.data.fd = STDIN_FILENO;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &event) == -1) {
        perror("epoll_ctl");
        return 1;
    }

    struct epoll_event events[MAX_EVENTS];

    while (1) {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_events == -1) {
            perror("epoll_wait");
            return 1;
        }

        for (int i = 0; i < num_events; i++) {
            if (events[i].data.fd == STDIN_FILENO) {
                char buf[256];
                ssize_t bytes_read = read(STDIN_FILENO, buf, sizeof(buf));
                if (bytes_read == -1) {
                    perror("read");
                    return 1;
                }
                printf("Read from stdin: %.*s", (int)bytes_read, buf);
            }
        }
    }

    close(epoll_fd);
    return 0;
}
