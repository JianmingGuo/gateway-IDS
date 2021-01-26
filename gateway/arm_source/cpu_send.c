#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define SERVER_PORT 55555

int main()
{
    FILE *fp;
    fp = fopen("cpu.txt", "w"); //empty file
    fclose(fp);
    int a = 0, i = 0, cnt = 0;
    int sub = 0, per = 0, sum = 0;
    int j[10] = {0}, j1[10] = {0}, j2[10] = {0};
    char c[1000];
    char cper[3];
    FILE *fptr;

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
            fp = fopen("cpu.txt", "w");
            fclose(fp);
            printf("reset.\n");
            sleep(1);
            continue;
        }
        printf("%d\n", a);
        a++;

        fptr = fopen("//proc//stat", "r");
        fgets(c, sizeof(c), fptr);
        i = 0;
        cnt = 0;
        memset(j1, 0, sizeof(j1));

        for (i = 5; c[i] != '\0'; i++)
        {
            if (c[i] >= 48 && c[i] <= 57)
            {
                j1[cnt] = j1[cnt] * 10 + (c[i] - 48);
            }
            if (c[i] == ' ')
                cnt++;
        }
        fclose(fptr);

        for (int k = 0; k < 10; ++k)
            printf("%d\t", j1[k]);
        printf("%c", '\n');

        sleep(1);

        fptr = fopen("//proc//stat", "r");
        fgets(c, sizeof(c), fptr);
        i = 0;
        cnt = 0;
        memset(j2, 0, sizeof(j2));

        for (i = 5; c[i] != '\0'; i++)
        {
            if (c[i] >= 48 && c[i] <= 57)
            {
                j2[cnt] = j2[cnt] * 10 + (c[i] - 48);
            }
            if (c[i] == ' ')
                cnt++;
        }
        fclose(fptr);

        for (int k = 0; k < 10; ++k)
            printf("%d\t", j2[k]);
        printf("%c", '\n');

        memset(j, 0, sizeof(j));

        for (i = 0; i < 10; ++i)
            j[i] = j2[i] - j1[i];

        sub = 0;
        per = 0;
        for (int k = 0; k < 10; ++k)
            sub += j[k];
        per = 100 * (sub - j[3]) / sub;
        printf("%d", per);
        printf("%s", "%\n\n");

        sum = 0;
        for (int k = 0; k < 10; ++k)
            sum += j2[k];

        FILE *dst;
        dst = fopen("cpu.txt", "a+");
        if (dst == NULL)
        {
            printf("open destination file failed\n");
            return 0;
        }

        fprintf(dst, "%d,", per);
        fclose(dst);

        sprintf(cper, "%d", per);
        send(clientSocket, cper, strlen(cper), 0);
        memset(cper, 0, sizeof(cper));
    }

    close(clientSocket);
    return 0;
}
