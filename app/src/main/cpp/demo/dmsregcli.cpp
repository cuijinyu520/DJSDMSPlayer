//#include "dmsregcli.h"
#include <cstdio>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../../include/libdmsreg.h"

#define REG_SVC_PORT    1234
#define REG_SVC_ADDRESS "123.127.110.190"

/**
 * @brief 事件回调处理函数，用于接收并打印来自 libdmsreg 库的事件通知。
 *
 * @param iEvent 事件类型标识符（通常是一个32位整数）
 * @param pData 指向事件相关数据的指针（本例中未使用）
 */
void EventHandler(int iEvent, void* pData)
{
    printf("libdmsreg Event = 0x%08X\n", iEvent);
}


/**
 * @brief 执行设备注册流程。创建TCP套接字连接到指定服务器地址和端口，
 *        并持续监听、处理来自服务器的消息。
 *
 * @return 成功时返回0；失败时返回-1。
 */
int Register()
{
    int s_fd;                         ///< 套接字文件描述符
    int len = sizeof(struct sockaddr_in); ///< 地址结构体长度
    struct sockaddr_in s_addr;        ///< 服务端网络地址信息

    int n_read = 0;                   ///< 实际读取的数据长度
    int n_write = 0;                  ///< 实际发送的数据长度
    int mark = 0;                     ///< 标记变量（当前未被实际使用）
    uint8_t read_buffer[131072] = { 0 }; ///< 接收缓冲区，最大支持128KB数据
    uint8_t* write_buffer = NULL;     ///< 发送缓冲区指针，由库动态分配

    // 注册事件处理器
    _dms_register_event_handler(EventHandler);

    // 创建TCP套接字
    s_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (s_fd < 0)
    {
        printf("Create socket error\n");
        return -1;
    }
    else printf("Socket created\n");

    // 初始化服务器地址结构
    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(REG_SVC_PORT);
    //s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET, REG_SVC_ADDRESS, &s_addr.sin_addr.s_addr);

    // 获取并打印套接字发送与接收缓冲区大小
    int opt_val = 1024 * 512;
    socklen_t opt_len = sizeof(opt_val);
    //setsockopt(s_fd, SOL_SOCKET, SO_RCVBUF, (char*)&opt_val, opt_len);
    //setsockopt(s_fd, SOL_SOCKET, SO_SNDBUF, (char*)&opt_val, opt_len);
    if (getsockopt(s_fd, SOL_SOCKET, SO_SNDBUF, (char*)&opt_val, &opt_len) < 0)
        printf("fail to getsockopt\n");
    else
        printf("Socket send buffer length: %d byte(s)\n", opt_val);

    if (getsockopt(s_fd, SOL_SOCKET, SO_RCVBUF, (char*)&opt_val, &opt_len) < 0)
        printf("fail to getsockopt\n");
    else
        printf("Socket receive buffer length: %d byte(s)\n", opt_val);

    // 连接到远程服务器
    if (connect(s_fd, (struct sockaddr*)&s_addr, sizeof(s_addr)) == -1)
    {
        printf("Connect to [%s:%d] error\n", REG_SVC_ADDRESS, REG_SVC_PORT);
        return -1;
    }
    else
        printf("DmsRegServer [%s:%d] connected\n", REG_SVC_ADDRESS, REG_SVC_PORT);

    // 主循环：不断从套接字读取消息，并调用库进行消息处理及响应
    // 套接字（Socket）是网络编程中的一个抽象概念，用于描述网络连接的一端。
    // 它允许不同计算机上的进程之间通过网络进行数据交换。
    // 在此程序中，套接字用于与远程服务器建立TCP连接，并接收和发送数据。
    while (1)
    {
        memset(read_buffer, 0, sizeof(read_buffer));

        n_read = read(s_fd, read_buffer, sizeof(read_buffer));

        if (n_read == -1)
        {
            printf("DmsRegServer disconnected\n", inet_ntoa(s_addr.sin_addr), ntohs(s_addr.sin_port));
            close(s_fd);
            return 0;
        }
        else if (n_read > 0)
        {
            printf("Socket read length: %d\n", n_read);

            // 处理收到的消息并准备回复内容
            n_write = _dms_process_message(read_buffer, n_read, &write_buffer);
            if (write_buffer)
            {
                // 将响应写回服务器
                n_write = write(s_fd, write_buffer, n_write);
                printf("Socket write length: %d\n", n_write);
                _dms_free_message(write_buffer); // 释放库分配的内存
                write_buffer = NULL;
            }
        }
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief 程序主入口函数。显示程序启动Banner后执行注册逻辑。
 *
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组
 * @return 返回状态码（始终为0）
 */
int main(int argc, char* argv[])
{
    printf("  ________  _____ ______   ________  ________      ____  __  __ ____  ____             ____ _ _            _\n");
    printf(" |\\   ___ \\|\\   _ \\  _   \\|\\   ____\\|\\   ____\\    |  _ \\|  \\/  / ___||  _ \\ ___  __ _ / ___| (_) ___ _ __ | |_\n");
    printf(" \\ \\  \\_|\\ \\ \\  \\\\\\__\\ \\  \\ \\  \\___|\\ \\  \\___|    | | | | |\\/| \\___ \\| |_) / _ \\/ _` | |   | | |/ _ \\ '_ \\| __|\n");
    printf("  \\ \\  \\ \\\\ \\ \\  \\\\|__| \\  \\ \\  \\    \\ \\  \\       | |_| | |  | |___) |  _ <  __/ (_| | |___| | |  __/ | | | |_\n");
    printf("   \\ \\  \\_\\\\ \\ \\  \\    \\ \\  \\ \\  \\____\\ \\  \\____  |____/|_|  |_|____/|_| \\_\\___|\\__, |\\____|_|_|\\___|_| |_|\\__|\n");
    printf("    \\ \\_______\\ \\__\\    \\ \\__\\ \\_______\\ \\_______\\  Version 0.1                 |___/\n");
    printf("     \\|_______|\\|__|     \\|__|\\|_______|\\|_______| (C)Copyright DMCC, May.2025, Written by Liujiannan\n\n");
    
    Register();// 启动注册过程

    return 0;
}
