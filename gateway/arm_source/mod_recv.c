#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <mysql/mysql.h>
#define SERVER_PORT 29999

/*
监听后，一直处于accept阻塞状态，
直到有客户端连接，
当客户端如数quit后，断开与客户端的连接
*/
int main()
{

    //调用socket函数返回的文件描述符
    int serverSocket;
    //声明两个套接字sockaddr_in结构体变量，分别表示客户端和服务器
    struct sockaddr_in server_addr;
    struct sockaddr_in clientAddr;
    int addr_len = sizeof(clientAddr);
    int client;
    char buffer[200];
    int iDataNum;

    char c[200], v;
    char time[30] = "", mod[25] = "";
    int line = 0, cnt = 0, loop = 0;
    FILE *chg, *his;

    MYSQL *mysql;
    MYSQL_RES *mysql_res;
    MYSQL_ROW mysql_row;

    char sqlcmd[200];
    int r, t;
    char aa[3] = "aas", bb[3] = "BBs";

    mysql = mysql_init(NULL);
    // 链接
    mysql = mysql_real_connect(mysql, "39.100.88.210", "siov", "siov", "siov", 0, NULL, 0);
    mysql_set_character_set(mysql, "utf8");

    sprintf(sqlcmd, "delete from current_module");
    mysql_real_query(mysql, sqlcmd, strlen(sqlcmd));
    sprintf(sqlcmd, "delete from changed_module");
    mysql_real_query(mysql, sqlcmd, strlen(sqlcmd));

    chg = fopen("mod_recv.txt", "w");
    fclose(chg);
    his = fopen("his_recv.txt", "w");
    fclose(his);

    //socket函数，失败返回-1
    //int socket(int domain, int type, int protocol);
    //第一个参数表示使用的地址类型，一般都是ipv4，AF_INET
    //第二个参数表示套接字类型：tcp：面向连接的稳定数据传输SOCK_STREAM
    //第三个参数设置为0
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return 1;
    }

    bzero(&server_addr, sizeof(server_addr));

    //初始化服务器端的套接字，并用htons和htonl将端口和地址转成网络字节序
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    //ip可是是本服务器的ip，也可以用宏INADDR_ANY代替，代表0.0.0.0，表明所有地址
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //对于bind，accept之类的函数，里面套接字参数都是需要强制转换成(struct sockaddr *)
    //bind三个参数：服务器端的套接字的文件描述符，

    if (bind(serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        return 1;
    }

    //设置服务器上的socket为监听状态
    if (listen(serverSocket, 5) < 0)
    {
        perror("listen");
        return 1;
    }

    while (1)
    {

        printf("监听端口: %d\n", SERVER_PORT);

        //调用accept函数后，会进入阻塞状态
        //accept返回一个套接字的文件描述符，这样服务器端便有两个套接字的文件描述符，
        //serverSocket和client。
        //serverSocket仍然继续在监听状态，client则负责接收和发送数据
        //clientAddr是一个传出参数，accept返回时，传出客户端的地址和端口号
        //addr_len是一个传入-传出参数，传入的是调用者提供的缓冲区的clientAddr的长度，以避免缓冲区溢出。
        //传出的是客户端地址结构体的实际长度。
        //出错返回-1
        client = accept(serverSocket, (struct sockaddr *)&clientAddr, (socklen_t *)&addr_len);

        if (client < 0)
        {
            perror("accept");
            continue;
        }

        printf("等待消息...\n");
        //inet_ntoa ip地址转换函数，将网络字节序IP转换为点分十进制IP
        //表达式：char *inet_ntoa (struct in_addr);
        printf("IP is %s\n", inet_ntoa(clientAddr.sin_addr));
        printf("Port is %d\n", htons(clientAddr.sin_port));

        while (1)
        {
            //printf("读取消息:");
            buffer[0] = '\0';
            iDataNum = recv(client, buffer, 1024, 0);
            if (iDataNum < 0)
            {
                perror("recv null");
                continue;
            }

            buffer[iDataNum] = '\0';
            //printf("%s", buffer);

            chg = fopen("mod_recv.txt", "r");
            fseek(chg, -2, SEEK_END);
            v = fgetc(chg);
            if (v == '@')
            {
                sprintf(sqlcmd, "delete from changed_module");
                mysql_real_query(mysql, sqlcmd, strlen(sqlcmd));

                printf("To be renewed.\n");
                rewind(chg);

                while (fgets(c, sizeof(c), chg) != NULL)
                {
                    if (c[0] == '@')
                        break;
                    int cnt = 0, pos = 0, ifAct;
                    for (int i = 0; c[i] != '\n'; i++)
                    {
                        if (c[i] == '\t')
                        {
                            cnt++;
                            pos = 0;
                            continue;
                        }
                        switch (cnt)
                        {
                        case 0:
                            time[pos++] = c[i];
                            break;
                        case 1:
                            if (c[i] == '+')
                                ifAct = 1;
                            else
                                ifAct = 0;
                            cnt++;
                            break;
                        case 2:
                            mod[pos++] = c[i];
                            break;
                        }
                    }

                    // printf("%s,%d,%s\n",mod,ifAct,time);

                    // 插入mod_cur表
                    sprintf(sqlcmd, "select count(*) from PT_module where module='%s'", mod);
                    mysql_real_query(mysql, sqlcmd, strlen(sqlcmd));

                    //查询开始
                    mysql_res = mysql_store_result(mysql);
                    while ((mysql_row = mysql_fetch_row(mysql_res)))
                    {
                        if (ifAct)
                        {
                            if (strcmp(mysql_row[0], "0") == 0)
                            {
                                sprintf(sqlcmd, "insert into current_module values('%s','%s','abnormal')", mod, time);
                                mysql_real_query(mysql, sqlcmd, strlen(sqlcmd));
                                sprintf(sqlcmd, "insert into changed_module values('activate', '%s','%s','abnormal')", mod, time);
                                mysql_real_query(mysql, sqlcmd, strlen(sqlcmd));
                            }
                            else
                            {
                                sprintf(sqlcmd, "insert into current_module values('%s','%s','normal')", mod, time);
                                mysql_real_query(mysql, sqlcmd, strlen(sqlcmd));
                                sprintf(sqlcmd, "insert into changed_module values('activate', '%s','%s','normal')", mod, time);
                                mysql_real_query(mysql, sqlcmd, strlen(sqlcmd));
                            }
                        }
                        else
                        {
                            sprintf(sqlcmd, "delete from current_module where module='%s'", mod);
                            mysql_real_query(mysql, sqlcmd, strlen(sqlcmd));
                            if (strcmp(mysql_row[0], "0") == 0)
                            {
                                sprintf(sqlcmd, "insert into changed_module values('deactivate', '%s','%s','abnormal')", mod, time);
                                mysql_real_query(mysql, sqlcmd, strlen(sqlcmd));
                            }
                            else
                            {
                                sprintf(sqlcmd, "insert into changed_module values('deactivate', '%s','%s','normal')", mod, time);
                                mysql_real_query(mysql, sqlcmd, strlen(sqlcmd));
                            }
                        }
                    }
                    // 释放
                    mysql_free_result(mysql_res);
                    memset(c, 0, sizeof(c));
                    memset(sqlcmd, 0, sizeof(sqlcmd));
                    memset(mod, 0, sizeof(mod));
                    memset(time, 0, sizeof(time));
                }

                fclose(chg);
                chg = fopen("mod_recv.txt", "w");
                fclose(chg);
            }

            chg = fopen("mod_recv.txt", "a+");
            fprintf(chg, "%s", buffer);
            fclose(chg);
            his = fopen("his_recv.txt", "a+");
            fprintf(his, "%s", buffer);
            fclose(his);

            /*if (v == 'h') //his
                {
                    his = fopen("his_recv.txt", "a+");
                    fprintf(his, "%s", c);
                    fclose(his);
                }

                if (v == 't') //text
                {
                    text = fopen("text_recv.txt", "a+");
                    fprintf(text, "%s", c);
                    fclose(text);
                }*/
        }
    }
    mysql_close(mysql);
    close(serverSocket);
    return 0;
}
