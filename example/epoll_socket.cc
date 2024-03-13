#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define MAX_EVENTS 10
#define PORT 8080

int main() {
    int server_fd, client_fd, epoll_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    struct epoll_event event, events[MAX_EVENTS];


// 1.-------------------------------------------------------------------------------------------------------
    // 创建 socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 绑定地址和端口
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // 监听连接
    if (listen(server_fd, 5) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //封装到了Socket中
// -------------------------------------------------------------------------------------------------------
    
    // 创建 epoll 实例
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // 将监听套接字加入到 epoll 实例中
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        perror("epoll_ctl: server_fd");
        exit(EXIT_FAILURE);
    }

    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; i++) {
            //-----------------------------------------------------------------------------------------------------------
            if (events[i].data.fd == server_fd) {
                // 接受新连接
                client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
                if (client_fd == -1) {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                // 将新连接加入到 epoll 实例中
                event.events = EPOLLIN;
                event.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
                    perror("epoll_ctl: client_fd");
                    exit(EXIT_FAILURE);
                }
            } else {
                // 处理客户端发送的数据
                char buffer[1024];
                int bytes_received = recv(events[i].data.fd, buffer, sizeof(buffer), 0);
                if (bytes_received <= 0) {
                    close(events[i].data.fd);
                } else {
                    buffer[bytes_received] = '\0';
                    printf("Received: %s", buffer);
                    // 回复客户端
                    send(events[i].data.fd, buffer, strlen(buffer), 0);
                }
                //封装到了TcpConnection
            //-----------------------------------------------------------------------------------------------------------
            }
        }
    }
    close(server_fd);
    return 0;
}
