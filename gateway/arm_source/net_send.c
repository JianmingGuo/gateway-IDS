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

#define SERVER_PORT 6666

int main()
{
    FILE *fp, *fv4, *fv6;
    char c[200], v;
    int n = 0, cnt = 0, i;
    char ip[40] = "", por[32] = "";
    char rip[40] = "", rpor[32] = "";
    char v4[200] = "", v6[400] = "";
    int Ip[4] = {0}, Por = 0;
    int rIp[4] = {0}, rPor = 0;
    int flag = 0;
    int line = 0, skip = 0;

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

    //printf("连接到主机...\n");

    while (1)
    {
        fp = fopen("net.txt", "w"); //清空文件
        fclose(fp);
	memset(ip, 0, sizeof(ip));
	memset(rip, 0, sizeof(rip));
        memset(por, 0, sizeof(por));
        memset(rpor, 0, sizeof(rpor));

        //tcp
        fp = fopen("//proc//net//tcp", "r");

        while (cnt < 1 && (v = fgetc(fp)) != EOF) //跳过一行
            if (v == '\n')
                cnt++;

        cnt = 0;
        if (fp != NULL)
            while (fgets(c, sizeof(c), fp) != NULL)
            {
                //printf("%s", c);

                for (i = 0; i < 200; i++)
                {

                    if (c[i] == ':')
                    {
                        cnt++;
                        flag = 1;
                    }

                    if (cnt == 2 && flag) //获取本地ip地址和端口号
                    {
                        n = 0;
                        for (int j = i - 8; j < i; j++)
                            ip[n++] = c[j];
                        n = 0;
                        for (int j = i + 1; j < i + 5; j++)
                            por[n++] = c[j];
                        flag = 0;
                    }

                    if (cnt == 3 && flag) //获取远程ip地址和端口号
                    {
                        n = 0;
                        for (int j = i - 8; j < i; j++)
                            rip[n++] = c[j];
                        n = 0;
                        for (int j = i + 1; j < i + 5; j++)
                            rpor[n++] = c[j];
                        flag = 0;
                        break;
                    }
                }

                //跳过全0的IP地址
                for (n = 0; rip[n] != '\0'; n++)
                    if (rip[n] != '0')
                    {
                        flag = 1;
                        break;
                    }
                if (flag == 0)
                {
                    cnt = 0;
                    continue;
                }

                //将十六进制字符串转化为十进制数
                for (n = 0; ip[n] != '\0'; n++)
                {
                    if (ip[n] >= 'A' && ip[n] <= 'F')
                        Ip[n / 2] = 16 * Ip[n / 2] + ip[n] - 'A' + 10;
                    if (ip[n] >= '0' && ip[n] <= '9')
                        Ip[n / 2] = 16 * Ip[n / 2] + ip[n] - '0';
                }

                for (n = 0; por[n] != '\0'; n++)
                {
                    if (por[n] >= 'A' && por[n] <= 'F')
                        Por = 16 * Por + por[n] - 'A' + 10;
                    if (por[n] >= '0' && por[n] <= '9')
                        Por = 16 * Por + por[n] - '0';
                }

                for (n = 0; rip[n] != '\0'; n++)
                {
                    if (rip[n] >= 'A' && rip[n] <= 'F')
                        rIp[n / 2] = 16 * rIp[n / 2] + rip[n] - 'A' + 10;
                    if (rip[n] >= '0' && rip[n] <= '9')
                        rIp[n / 2] = 16 * rIp[n / 2] + rip[n] - '0';
                }

                for (n = 0; rpor[n] != '\0'; n++)
                {
                    if (rpor[n] >= 'A' && rpor[n] <= 'F')
                        rPor = 16 * rPor + rpor[n] - 'A' + 10;
                    if (rpor[n] >= '0' && rpor[n] <= '9')
                        rPor = 16 * rPor + rpor[n] - '0';
                }

                //跳过端口为6666的传输连接
                if (Por == 6666 || rPor == 6666)
                {
                    for (n = 0; n < 4; ++n)
                        Ip[n] = rIp[n] = 0;
                    Por = rPor = 0;
                    cnt = 0;
                    memset(por, 0, sizeof(por));
                    memset(rpor, 0, sizeof(rpor));
                    continue;
                }
		//printf("ip:\t%s\trip:\t%s\n",ip,rip);
                printf("\ttcp:\t%d.%d.%d.%d:%d\t", Ip[3], Ip[2], Ip[1], Ip[0], Por);
                printf("%d.%d.%d.%d:%d\n", rIp[3], rIp[2], rIp[1], rIp[0], rPor);

                //写入文件
                FILE *dst;
                dst = fopen("net.txt", "a+");
                if (dst == NULL)
                {
                    printf("open destination file failed\n");
                    return 0;
                }

                fprintf(dst, "tcp\t%d.%d.%d.%d:%d\t", Ip[3], Ip[2], Ip[1], Ip[0], Por);
                fprintf(dst, "%d.%d.%d.%d:%d\t", rIp[3], rIp[2], rIp[1], rIp[0], rPor);
                //输出系统时间
                time_t *timep = malloc(sizeof(*timep));
                time(timep);
                char *s = ctime(timep);
                fprintf(dst, "%s", s);
                fclose(dst);

                //生成用于传输的字符串
                fv4 = fopen("net.txt", "r");
                while (skip < line && (v = fgetc(fv4)) != EOF) //跳过一行
                    if (v == '\n')
                        skip++;
                skip = 0;
                if (fgets(v4, sizeof(v4), fv4) != NULL)
                    printf("v4:\t%s\n", v4);
                fclose(fv4);
                //传输
                send(clientSocket, v4, strlen(v4), 0);
                //重置
                for (n = 0; n < 4; ++n)
                    Ip[n] = rIp[n] = 0;
                Por = rPor = 0;
                cnt = 0;
                //printf("local:%s\tremote:%s",por,rpor);
                memset(v4, 0, sizeof(v4));
		memset(ip, 0, sizeof(ip));
		memset(rip, 0, sizeof(rip));
                memset(por, 0, sizeof(por));
                memset(rpor, 0, sizeof(rpor));
                line++;
            }
        fclose(fp);

        //udp
        fp = fopen("//proc//net//udp", "r");

        while (cnt < 1 && (v = fgetc(fp)) != EOF) //跳过一行
            if (v == '\n')
                cnt++;

        cnt = 0;
        if (fp != NULL)
            while (fgets(c, sizeof(c), fp) != NULL)
            {
                //printf("%s", c);

                for (i = 0; i < 200; i++)
                {

                    if (c[i] == ':')
                    {
                        cnt++;
                        flag = 1;
                    }

                    if (cnt == 2 && flag) //获取本地ip地址和端口号
                    {
                        n = 0;
                        for (int j = i - 8; j < i; j++)
                            ip[n++] = c[j];
                        n = 0;
                        for (int j = i + 1; j < i + 5; j++)
                            por[n++] = c[j];
                        flag = 0;
                    }

                    if (cnt == 3 && flag) //获取远程ip地址和端口号
                    {
                        n = 0;
                        for (int j = i - 8; j < i; j++)
                            rip[n++] = c[j];
                        n = 0;
                        for (int j = i + 1; j < i + 5; j++)
                            rpor[n++] = c[j];
                        flag = 0;
                        break;
                    }
                }

                //跳过全0的IP地址
                for (n = 0; rip[n] != '\0'; n++)
                    if (rip[n] != '0')
                    {
                        flag = 1;
                        break;
                    }
                if (flag == 0)
                {
                    cnt = 0;
                    continue;
                }

                //将十六进制字符串转化为十进制数
                for (n = 0; ip[n] != '\0'; n++)
                {
                    if (ip[n] >= 'A' && ip[n] <= 'F')
                        Ip[n / 2] = 16 * Ip[n / 2] + ip[n] - 'A' + 10;
                    if (ip[n] >= '0' && ip[n] <= '9')
                        Ip[n / 2] = 16 * Ip[n / 2] + ip[n] - '0';
                }

                for (n = 0; por[n] != '\0'; n++)
                {
                    if (por[n] >= 'A' && por[n] <= 'F')
                        Por = 16 * Por + por[n] - 'A' + 10;
                    if (por[n] >= '0' && por[n] <= '9')
                        Por = 16 * Por + por[n] - '0';
                }

                for (n = 0; rip[n] != '\0'; n++)
                {
                    if (rip[n] >= 'A' && rip[n] <= 'F')
                        rIp[n / 2] = 16 * rIp[n / 2] + rip[n] - 'A' + 10;
                    if (rip[n] >= '0' && rip[n] <= '9')
                        rIp[n / 2] = 16 * rIp[n / 2] + rip[n] - '0';
                }

                for (n = 0; rpor[n] != '\0'; n++)
                {
                    if (rpor[n] >= 'A' && rpor[n] <= 'F')
                        rPor = 16 * rPor + rpor[n] - 'A' + 10;
                    if (rpor[n] >= '0' && rpor[n] <= '9')
                        rPor = 16 * rPor + rpor[n] - '0';
                }
                //printf("%s\n", rip);
		//printf("ip:\t%s\trip:\t%s\n",ip,rip);
                printf("\tudp:\t%d.%d.%d.%d:%d\t", Ip[3], Ip[2], Ip[1], Ip[0], Por);
                printf("%d.%d.%d.%d:%d\n", rIp[3], rIp[2], rIp[1], rIp[0], rPor);

                //写入文件
                FILE *dst;
                dst = fopen("net.txt", "a+");
                if (dst == NULL)
                {
                    printf("open destination file failed\n");
                    return 0;
                }

                fprintf(dst, "udp\t%d.%d.%d.%d:%d\t", Ip[3], Ip[2], Ip[1], Ip[0], Por);
                fprintf(dst, "%d.%d.%d.%d:%d\t", rIp[3], rIp[2], rIp[1], rIp[0], rPor);
                //输出系统时间
                time_t *timep = malloc(sizeof(*timep));
                time(timep);
                char *s = ctime(timep);
                fprintf(dst, "%s", s);
                fclose(dst);
                
                //生成用于传输的字符串
                fv4 = fopen("net.txt", "r");
                while (skip < line && (v = fgetc(fv4)) != EOF) //跳过一行
                    if (v == '\n')
                        skip++;
                skip = 0;
                if (fgets(v4, sizeof(v4), fv4) != NULL)
                    printf("v4:\t%s\n", v4);
                fclose(fv4);
                //传输
                send(clientSocket, v4, strlen(v4), 0);

                //重置
                for (n = 0; n < 4; ++n)
                    Ip[n] = rIp[n] = 0;
                Por = rPor = 0;
                cnt = 0;
		memset(v4, 0, sizeof(v4));
                memset(ip, 0, sizeof(ip));
		memset(rip, 0, sizeof(rip));
                memset(por, 0, sizeof(por));
                memset(rpor, 0, sizeof(rpor));
                line++;
            }
        fclose(fp);

        //tcp6
        fp = fopen("//proc//net//tcp6", "r");

        while (cnt < 1 && (v = fgetc(fp)) != EOF) //跳过一行
            if (v == '\n')
                cnt++;

        cnt = 0;
        if (fp != NULL)
            while (fgets(c, sizeof(c), fp) != NULL)
            {
                //printf("%s", c);

                for (i = 0; i < 200; i++)
                {

                    if (c[i] == ':')
                    {
                        cnt++;
                        flag = 1;
                    }

                    if (cnt == 2 && flag) //获取本地ip地址和端口号
                    {
                        n = 0;
                        for (int j = i - 32; j < i; j++)
                            ip[n++] = c[j];
                        n = 0;
                        for (int j = i + 1; j < i + 5; j++)
                            por[n++] = c[j];
                        flag = 0;
                    }

                    if (cnt == 3 && flag) //获取远程ip地址和端口号
                    {
                        n = 0;
                        for (int j = i - 32; j < i; j++)
                            rip[n++] = c[j];
                        n = 0;
                        for (int j = i + 1; j < i + 5; j++)
                            rpor[n++] = c[j];
                        flag = 0;
                        break;
                    }
                }

                //跳过全0的IP地址
                for (n = 0; n < 32; n++)
                    if (rip[n] != '0')
                    {
                        flag = 1;
                        break;
                    }
                if (flag == 0)
                {
                    cnt = 0;
                    continue;
                }

                //将十六进制字符串转化为十进制数
                for (n = 0; por[n] != '\0'; n++)
                {
                    if (por[n] >= 'A' && por[n] <= 'F')
                        Por = 16 * Por + por[n] - 'A' + 10;
                    if (por[n] >= '0' && por[n] <= '9')
                        Por = 16 * Por + por[n] - '0';
                }
                for (n = 0; rpor[n] != '\0'; n++)
                {
                    if (rpor[n] >= 'A' && rpor[n] <= 'F')
                        rPor = 16 * rPor + rpor[n] - 'A' + 10;
                    if (rpor[n] >= '0' && rpor[n] <= '9')
                        rPor = 16 * rPor + rpor[n] - '0';
                }
		//printf("ip:\t%s\trip:\t%s\n",ip,rip);
                printf("\ttcp6:\t");
                //IPv6地址直接输出
                for (n = 7; n >= 0; n--)
                {
                    for (int j = 4 * n; j < 4 * n + 4; j++)
                        printf("%c", ip[j]);
                    if (n != 0)
                        printf(":");
                }
                printf("\t%d\t", Por);
                for (n = 7; n >= 0; n--)
                {
                    for (int j = 4 * n; j < 4 * n + 4; j++)
                        printf("%c", rip[j]);
                    if (n != 0)
                        printf(":");
                }
                printf("\t%d\n", rPor);

                //写入文件
                FILE *dst;
                dst = fopen("net.txt", "a+");
                if (dst == NULL)
                {
                    printf("open destination file failed\n");
                    return 0;
                }

                fprintf(dst, "tcp6\t");
                //IPv6地址直接输出
                for (n = 7; n >= 0; n--)
                {
                    for (int j = 4 * n; j < 4 * n + 4; j++)
                        fprintf(dst, "%c", ip[j]);
                    if (n != 0)
                        fprintf(dst, ":");
                }
                fprintf(dst, ":%d\t", Por);
                for (n = 7; n >= 0; n--)
                {
                    for (int j = 4 * n; j < 4 * n + 4; j++)
                        fprintf(dst, "%c", rip[j]);
                    if (n != 0)
                        fprintf(dst, ":");
                }
                fprintf(dst, ":%d\t", rPor);
                //输出系统时间
                time_t *timep = malloc(sizeof(*timep));
                time(timep);
                char *s = ctime(timep);
                fprintf(dst, "%s", s);
                fclose(dst);

                //生成用于传输的字符串
                fv6 = fopen("net.txt", "r");
                while (skip < line && (v = fgetc(fv6)) != EOF) //跳过一行
                    if (v == '\n')
                        skip++;
                skip = 0;
                if (fgets(v6, sizeof(v6), fv6) != NULL)
                    printf("v6:\t%s\n", v6);
                send(clientSocket, v6, strlen(v6), 0);

                fclose(fv6);
                //重置
                Por = rPor = 0;
                cnt = 0;
                memset(v6, 0, sizeof(v6));
		memset(ip, 0, sizeof(ip));
		memset(rip, 0, sizeof(rip));
                memset(por, 0, sizeof(por));
                memset(rpor, 0, sizeof(rpor));
                line++;
            }
        fclose(fp);

        //udp6
        fp = fopen("//proc//net//udp6", "r");

        while (cnt < 1 && (v = fgetc(fp)) != EOF) //跳过一行
            if (v == '\n')
                cnt++;

        cnt = 0;
        if (fp != NULL)
            while (fgets(c, sizeof(c), fp) != NULL)
            {
                //printf("%s", c);

                for (i = 0; i < 200; i++)
                {

                    if (c[i] == ':')
                    {
                        cnt++;
                        flag = 1;
                    }

                    if (cnt == 2 && flag) //获取本地ip地址和端口号
                    {
                        n = 0;
                        for (int j = i - 32; j < i; j++)
                            ip[n++] = c[j];
                        n = 0;
                        for (int j = i + 1; j < i + 5; j++)
                            por[n++] = c[j];
                        flag = 0;
                    }

                    if (cnt == 3 && flag) //获取远程ip地址和端口号
                    {
                        n = 0;
                        for (int j = i - 32; j < i; j++)
                            rip[n++] = c[j];
                        n = 0;
                        for (int j = i + 1; j < i + 5; j++)
                            rpor[n++] = c[j];
                        flag = 0;
                        break;
                    }
                }

                //跳过全0的IP地址
                for (n = 0; n < 32; n++)
                    if (rip[n] != '0')
                    {
                        flag = 1;
                        break;
                    }
                if (flag == 0)
                {
                    cnt = 0;
                    continue;
                }

                //将十六进制字符串转化为十进制数
                for (n = 0; por[n] != '\0'; n++)
                {
                    if (por[n] >= 'A' && por[n] <= 'F')
                        Por = 16 * Por + por[n] - 'A' + 10;
                    if (por[n] >= '0' && por[n] <= '9')
                        Por = 16 * Por + por[n] - '0';
                }
                for (n = 0; rpor[n] != '\0'; n++)
                {
                    if (rpor[n] >= 'A' && rpor[n] <= 'F')
                        rPor = 16 * rPor + rpor[n] - 'A' + 10;
                    if (rpor[n] >= '0' && rpor[n] <= '9')
                        rPor = 16 * rPor + rpor[n] - '0';
                }
		//printf("ip:\t%s\trip:\t%s\n",ip,rip);
                printf("\tudp6:\t");
                //IPv6地址直接输出
                for (n = 7; n >= 0; n--)
                {
                    for (int j = 4 * n; j < 4 * n + 4; j++)
                        printf("%c", ip[j]);
                    if (n != 0)
                        printf(":");
                }
                printf("\t%d\t", Por);
                for (n = 7; n >= 0; n--)
                {
                    for (int j = 4 * n; j < 4 * n + 4; j++)
                        printf("%c", rip[j]);
                    if (n != 0)
                        printf(":");
                }
                printf("\t%d\n", rPor);

                //写入文件
                FILE *dst;
                dst = fopen("net.txt", "a+");
                if (dst == NULL)
                {
                    printf("open destination file failed\n");
                    return 0;
                }

                fprintf(dst, "udp6\t");
                //IPv6地址直接输出
                for (n = 7; n >= 0; n--)
                {
                    for (int j = 4 * n; j < 4 * n + 4; j++)
                        fprintf(dst, "%c", ip[j]);
                    if (n != 0)
                        fprintf(dst, ":");
                }
                fprintf(dst, "\t%d\t", Por);
                for (n = 7; n >= 0; n--)
                {
                    for (int j = 4 * n; j < 4 * n + 4; j++)
                        fprintf(dst, "%c", rip[j]);
                    if (n != 0)
                        fprintf(dst, ":");
                }
                fprintf(dst, "\t%d\t", rPor);
                //输出系统时间
                time_t *timep = malloc(sizeof(*timep));
                time(timep);
                char *s = ctime(timep);
                fprintf(dst, "%s", s);
                fclose(dst);

                //生成用于传输的字符串
                fv6 = fopen("net.txt", "r");
                while (skip < line && (v = fgetc(fv6)) != EOF) //跳过一行
                    if (v == '\n')
                        skip++;
                skip = 0;
                if (fgets(v6, sizeof(v6), fv6) != NULL)
                    printf("v6:\t%s\n", v6);
                send(clientSocket, v6, strlen(v6), 0);
                fclose(fv6);
                //重置
                Por = rPor = 0;
                cnt = 0;
                memset(v6, 0, sizeof(v6));
		memset(ip, 0, sizeof(ip));
		memset(rip, 0, sizeof(rip));
                memset(por, 0, sizeof(por));
                memset(rpor, 0, sizeof(rpor));
                line++;
            }
        fclose(fp);
        send(clientSocket, "@", 1, 0);
        sleep(10);
        line = 0;
    }

    close(clientSocket);
    return 0;
}
