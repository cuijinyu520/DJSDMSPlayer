#include <stdio.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../../include/libdmsreg.h"

/**
 * @brief 服务注册端口定义
 */
#define REG_SVC_PORT    7815

/**
 * @brief 服务监听地址定义
 */
#define REG_SVC_ADDRESS "0.0.0.0"

/**
 * @brief 注册服务主函数，创建TCP服务端并处理客户端连接请求
 *
 * 该函数创建一个TCP服务端套接字，绑定到指定端口并监听客户端连接。
 * 当有客户端连接时，创建子进程处理客户端消息的读取和响应。
 *
 * @return int 返回0表示服务正常运行，返回-1表示发生错误
 */
int RegisterService()
{
    int s_fd;                           // 服务端套接字文件描述符
    int c_fd;                           // 客户端连接套接字文件描述符
    int len = sizeof(struct sockaddr_in); // 地址结构体长度
    struct sockaddr_in s_addr;          // 服务端地址信息
    struct sockaddr_in c_addr;          // 客户端地址信息

    int n_read = 0;                     // 读取数据长度
    int n_write = 0;                    // 写入数据长度
    int mark = 0;                       // 标记变量（未使用）
    uint8_t read_buffer[131072] = { 0 }; // 读取缓冲区，大小为128KB
    uint8_t* write_buffer = NULL;       // 写入缓冲区指针

    // 创建TCP套接字
    s_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (s_fd < 0)
    {
        printf("Create socket error\n");
        return -1;
    }
    else printf("Socket created\n");

    // 初始化服务端地址结构体
    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(REG_SVC_PORT);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //inet_pton(AF_INET, REG_SVC_ADDRESS, &s_addr.sin_addr.s_addr);

    // 获取并打印套接字发送和接收缓冲区大小
    int opt_val = 1024 * 512;
    socklen_t opt_len = sizeof(opt_val);
    //setsockopt(s_fd, SOL_SOCKET, SO_RCVBUF, (char*)&opt_val, opt_len);
    //setsockopt(s_fd, SOL_SOCKET, SO_SNDBUF, (char*)&opt_val, opt_len);
    if (getsockopt(s_fd, SOL_SOCKET, SO_SNDBUF, (char*)&opt_val, &opt_len) < 0) printf("fail to getsockopt\n");
    else printf("Socket send buffer length: %d byte(s)\n", opt_val);
    if (getsockopt(s_fd, SOL_SOCKET, SO_RCVBUF, (char*)&opt_val, &opt_len) < 0) printf("fail to getsockopt\n");
    else printf("Socket receive buffer length: %d byte(s)\n", opt_val);

    // 绑定套接字到指定地址和端口
    if (bind(s_fd, (struct sockaddr*)&s_addr, sizeof(struct sockaddr_in)) != 0)
    {
        printf("Bind socket error\n");
        return -1;
    }
    else printf("Socket bind on [%s:%d]\n", inet_ntoa(s_addr.sin_addr), ntohs(s_addr.sin_port));

    // 开始监听客户端连接
    if (listen(s_fd, 10) != 0)
    {
        printf("Listen error\n");
        return -1;
    }
    else printf("Socket listening ...\n");

    // 循环接受客户端连接
    while (1)
    {
        c_fd = accept(s_fd, (struct sockaddr*)&c_addr, (socklen_t*)&len);
        if (c_fd == -1) 
        {
            printf("Accept client error\n");
            return -1;
        }
        printf("Client [%s:%d] connected\n", inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));

        // 创建子进程处理客户端请求
        if (fork() == 0)
        {
            // 子进程循环处理客户端消息
            while (1)
            {
                // 从客户端读取数据
                memset(read_buffer, 0, sizeof(read_buffer));

                n_read = read(c_fd, read_buffer, sizeof(read_buffer));

                // 客户端断开连接
                if (n_read == -1)
                {
                    printf("Client [%s:%d] disconnected\n", inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));
                    close(c_fd);
                    return 0;
                }
                else if (n_read > 0)
                {
                    // 处理接收到的消息并生成响应
                    printf("Socket read length: %d\n", n_read);

                    n_write = _dms_process_message(read_buffer, n_read, &write_buffer);
                    if (write_buffer)
                    {
                        // 向客户端发送响应数据
                        n_write = write(c_fd, write_buffer, n_write);
                        printf("Socket write length: %d\n", n_write);
                        _dms_free_message(write_buffer);
                        write_buffer = NULL;
                    }
                }
            }
            break;
        }
    }

    return 0;
}


////////////////////////////////////////////////////////////////////////////////
/**
 * @brief 程序入口函数，打印欢迎信息并启动注册服务
 *
 * 该函数首先打印程序的ASCII艺术Logo和版本信息，
 * 然后启用日志功能并调用RegisterService函数启动服务。
 *
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组
 * @return int 返回0表示程序正常退出
 */
int main(int argc, char* argv[])
{
    printf("  ________  _____ ______   ________  ________      ____  __  __ ____  ____            ____ \n");
    printf(" |\\   ___ \\|\\   _ \\  _   \\|\\   ____\\|\\   ____\\    |  _ \\|  \\/  / ___||  _ \\ ___  __ _/ ___|_   _____\n");
    printf(" \\ \\  \\_|\\ \\ \\  \\\\\\__\\ \\  \\ \\  \\___|\\ \\  \\___|    | | | | |\\/| \\___ \\| |_) / _ \\/ _` \\___ \\ \\ / / __|\n");
    printf("  \\ \\  \\ \\\\ \\ \\  \\\\|__| \\  \\ \\  \\    \\ \\  \\       | |_| | |  | |___) |  _ <  __/ (_| |___) \\ V / (__ \n");
    printf("   \\ \\  \\_\\\\ \\ \\  \\    \\ \\  \\ \\  \\____\\ \\  \\____  |____/|_|  |_|____/|_| \\_\\___|\\__, |____/ \\_/ \\___|\n");
    printf("    \\ \\_______\\ \\__\\    \\ \\__\\ \\_______\\ \\_______\\  Version 0.2                 |___/\n");
    printf("     \\|_______|\\|__|     \\|__|\\|_______|\\|_______| (C)Copyright DMCC, Nov.2022, Written by Liujiannan\n\n");

    // 打印库版本信息并启用日志
    printf("DMS Device Register Library Version %s\n", _dms_get_library_version());
    _dms_enable_log(true);

    // 启动注册服务
    RegisterService();

    return 0;
}
