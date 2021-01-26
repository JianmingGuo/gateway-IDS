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

#define SERVER_PORT 7777

int main()
{
    FILE *dst;
    dst = fopen("disk.txt", "w");
    fclose(dst);

    struct statfs diskInfo;
    statfs("/", &diskInfo);
    unsigned long long total;
    unsigned long long avail;
    int per;
    char disk[200]="";

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
        dst = fopen("disk.txt", "w");
        fclose(dst);

        total = diskInfo.f_blocks;//reset
        avail = diskInfo.f_bavail;
        per = (total - avail) * 100 / total;
        printf("%d%%\n", per);

        time_t *timep;
        char *t;
        timep = malloc(sizeof(*timep));
        time(timep);
        t = ctime(timep);

        dst = fopen("disk.txt", "a+");
        fprintf(dst, "%d%%\t%s", per, t);
        fclose(dst);

        sprintf(disk,"%d%%\t",per);
        strcat(disk,t);
        printf("%s\n",disk);
        send(clientSocket, disk, strlen(disk), 0);

        //重置
        memset(disk,0,sizeof(disk));
        memset(t,0,sizeof(t));
        per = 0;
        free(timep);

        sleep(60);
    }

    close(clientSocket);
    return 0;
}
