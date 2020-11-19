#include <sys/vfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define SERVER_PORT 12345

int main()
{
    //客户端只需要一个套接字文件描述符，用于和服务器通信
    int clientSocket;
 
    //描述服务器的socket
    struct sockaddr_in serverAddr;
    int iDataNum;
 
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return 1;
 
    }
 
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
 
    //指定服务器端的ip，本地测试：127.0.0.1
    //inet_addr()函数，将点分十进制IP转换成网络字节序IP
    serverAddr.sin_addr.s_addr = inet_addr("192.168.1.109");
    if (connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
 
        perror("connect");
 
        return 1;
 
    }

    while (1)
    {
        ;
    }

    close(clientSocket);
    return 0;
}
