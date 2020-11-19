#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#define SERVER_PORT 8888

int main()
{
    FILE *fp, *dst;
    fp = fopen("mem.txt", "w"); //empty file
    fclose(fp);

    int a = 0;
    char c[1000];

    int cnt = 0;
    int j[3] = {0};
    int total = 0, avail = 0;
    int per = 0;
    char v;
    char mper[3];

    //客户端只需要一个套接字文件描述符，用于和服务器通信
    int clientSocket;

    //描述服务器的socket
    struct sockaddr_in serverAddr;
    int iDataNum;

    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);

    //指定服务器端的ip，本地测试：127.0.0.1
    //inet_addr()函数，将点分十进制IP转换成网络字节序IP
    serverAddr.sin_addr.s_addr = inet_addr("192.168.1.109");
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {

        perror("connect");

        return 1;
    }

    while (1)
    {

        if (a == 36000) //overwrite
        {
            a = 0;
            FILE *fp;
            fp = fopen("mem.txt", "w");
            fclose(fp);
            printf("reset.\n");
            sleep(1);
            continue;
        }
        printf("%d\n\r", a);
        a++;

        if ((fp = fopen("//proc//meminfo", "r")) == NULL)
        {
            printf("Error! opening file");
            // 文件指针返回 NULL 则退出
            return 0;
        }

        cnt = 0;
        memset(j, 0, sizeof(j));

        for (int n = 0; n < 3; ++n)
        {
            while (cnt < n && (v = fgetc(fp)) != EOF) //n就是你想跳过的行数
                if (v == '\n')
                    cnt++;

            fscanf(fp, "%[^\n]", c);
            printf("%s\n\r", c);

            for (int p = 0; c[p] != '\0'; ++p)
                if ((c[p] >= 48) && (c[p] <= 57))
                    j[n] = j[n] * 10 + c[p] - 48;
        }

        fclose(fp);

        total = j[0];
        avail = j[2];

        per = 100 * (total - avail) / total;

        dst = fopen("mem.txt", "a+");
        if (dst == NULL)
        {
            printf("open destination file failed\n");
            return 0;
        }
        printf("%d%%\n\n", per);

        fprintf(dst, "%d,", per);
        fclose(dst);
        sprintf(mper, "%d", per);
        send(clientSocket, mper, strlen(mper), 0);
        memset(mper, 0, sizeof(mper));

        sleep(1);
    }

    close(clientSocket);

    return 0;
}
