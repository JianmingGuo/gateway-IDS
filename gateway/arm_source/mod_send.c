#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

#define SERVER_PORT 29999

int main()
{
    FILE *fp1, *fp2;
    FILE *dst, *dst1, *dst2, *text, *his;
    char c1[200] = "", c2[200] = "", mod[50] = "", ch1, ch2;
    char /*mod_text[50] = "",*/ mod_chg[200] = "", mod_his[200] = "";
    int flag = 0, i1 = 0, i2 = 0, j1 = 0, j2 = 0, i, id;
    time_t *timep;
    char *t;

    his = fopen("module_history.txt", "a+");
    fclose(his);

    //客户端只需要一个套接字文件描述符，用于和服务器通信
    int clientSocket;

    //描述服务器的socket
    struct sockaddr_in serverAddr;

    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket1");
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    //指定服务器端的ip，本地测试：127.0.0.1
    //inet_addr()函数，将点分十进制IP转换成网络字节序IP
    serverAddr.sin_addr.s_addr = inet_addr("192.168.1.109");
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("connect1");
        return 1;
    }

    while (1)
    { 
        dst = fopen("module.txt", "w"); //储存每次循环的结果
        fclose(dst);

        text = fopen("module_text.txt", "w"); //储存已激活的模块
        fclose(text);

        dst1 = fopen("temp1.txt", "a+"); //储存比较数据
        fclose(dst1);
        dst2 = fopen("temp2.txt", "a+");
        fclose(dst2);

        dst1 = fopen("temp1.txt", "r");
        ch1 = fgetc(dst1);
        dst2 = fopen("temp2.txt", "r");
        ch2 = fgetc(dst2);

        if ((ch1 < 0 || ch1 > 127) && (ch2 < 0 || ch2 > 127)) 
        {
            fclose(dst1);
            fclose(dst2);

            id = 0;
            fp1 = fopen("//proc//modules", "r");
            if (fp1 != NULL)
                while (fgets(c1, sizeof(c1), fp1) != NULL)
                {
                    for (int i = 0; c1[i] != ' '; i++)
                        mod[i] = c1[i];

                    dst1 = fopen("temp1.txt", "a+");
                    dst = fopen("module.txt", "a+");
                    text = fopen("module_text.txt", "a+");
                    if (dst == NULL)
                    {
                        printf("open destination file failed\n");
                        return 0;
                    }

                    //time
                    timep = malloc(sizeof(*timep));
                    time(timep);
                    t = ctime(timep);
                    t[strlen(t) - 1] = '\0';
                    fprintf(dst, "%s\t", t);

                    fprintf(dst1, "%s\n", mod);
                    fprintf(dst, "+%s\n", mod);
                    fprintf(text, "%s\n", mod);

                    fclose(dst1);
                    fclose(dst);
                    fclose(text);

                    //传输数据
                    //strcat(mod_chg,"c"); //c表示变化项
                    strcat(mod_chg, t);
                    strcat(mod_chg, "\t+");
                    strcat(mod_chg, mod);
                    strcat(mod_chg, "\n");
                    send(clientSocket, mod_chg, strlen(mod_chg), 0);

                    /*strcat(mod_text,"t"); //t表示当前项
                    strcat(mod_text, mod);
                    strcat(mod_text, "\n");
                    send(clientSocket, mod_text, strlen(mod_text), 0);*/

                    //重置
                    memset(mod, 0, sizeof(mod));
                    memset(mod_chg, 0, sizeof(mod_chg));
                    //memset(mod_text, 0, sizeof(mod_text));
                }
            fclose(fp1);
            printf("%d\n", id);
            // 写入历史文件
            dst = fopen("module.txt", "a+");
            if (dst != NULL)
                while (fgets(c1, sizeof(c1), dst) != NULL)
                {
                    his = fopen("module_history.txt", "a+");
                    fprintf(his, "%s", c1);
                    fclose(his);

                    //传输数据
                    /*strcat(mod_his,"h"); //h表示历史项
                    strcat(mod_his, c1);
                    send(clientSocket, mod_his, strlen(mod_his), 0);

                    memset(mod_his, 0, sizeof(mod_his));*/
                }
            memset(c1, 0, 200);
            fclose(dst);

            sleep(5);
            continue;
        }
        if (ch1 < 0 || ch1 > 127)
        {
            fclose(dst1);
            fclose(dst2);

            id = 1;
            fp1 = fopen("//proc//modules", "r");
            if (fp1 != NULL)
                while (fgets(c1, sizeof(c1), fp1) != NULL)
                {
                    for (int i = 0; c1[i] != ' '; i++)
                        mod[i] = c1[i];

                    dst1 = fopen("temp1.txt", "a+");
                    if (dst1 == NULL)
                    {
                        printf("open destination file failed\n");
                        return 0;
                    }

                    fprintf(dst1, "%s\n", mod);
                    fclose(dst1);

                    text = fopen("module_text.txt", "a+");
                    fprintf(text, "%s\n", mod);
                    fclose(text);

                    /*strcat(mod_text,"t");
                    strcat(mod_text, mod);
                    strcat(mod_text, "\n");
                    send(clientSocket, mod_text, strlen(mod_text), 0);*/

                    memset(mod, 0, sizeof(mod));
                    //memset(mod_text, 0, sizeof(mod_text));
                }
            fclose(fp1);
        }
        if (ch2 < 0 || ch2 > 127)
        {
            fclose(dst1);
            fclose(dst2);

            id = 2;
            fp2 = fopen("//proc//modules", "r");

            if (fp2 != NULL)
                while (fgets(c2, sizeof(c2), fp2) != NULL)
                {
                    for (int i = 0; c2[i] != ' '; i++)
                        mod[i] = c2[i];

                    dst2 = fopen("temp2.txt", "a+");
                    fprintf(dst2, "%s\n", mod);
                    fclose(dst2);

                    text = fopen("module_text.txt", "a+");
                    fprintf(text, "%s\n", mod);
                    fclose(text);

                    /*strcat(mod_text,"t");
                    strcat(mod_text, mod);
                    strcat(mod_text, "\n");
                    send(clientSocket, mod_text, strlen(mod_text), 0);*/

                    memset(mod, 0, sizeof(mod));
                    //memset(mod_text, 0, sizeof(mod_text));
                }
            fclose(fp2);
        }
        printf("%d\n", id);

        //compare
        fp2 = fopen("temp2.txt", "r");
        while (fgets(c2, sizeof(c2), fp2) != NULL)
        {
            fp1 = fopen("temp1.txt", "r");
            while (fgets(c1, sizeof(c1), fp1) != NULL)
            {
                j1++;
                if (j1 == i1)
                    strcat(c1, "\n");

                flag = 0;
                for (i = 0; c1[i] != '\n' && c2[i] != '\n'; i++)
                    if (c1[i] != c2[i])
                    {
                        flag = 1;
                        break;
                    }

                if (c1[i] != '\n' || c2[i] != '\n')
                    flag = 1;
                if (flag == 0)
                    break;
            }
            fclose(fp1);
            j1 = 0;
            if (flag)
            {
                if (id == 1)
                    printf("-");
                if (id == 2)
                    printf("+");
                printf("%s", c2);

                dst = fopen("module.txt", "a+");

                //time
                timep = malloc(sizeof(*timep));
                time(timep);
                t = ctime(timep);
                t[strlen(t) - 1] = '\0';
                fprintf(dst, "%s\t", t);

                if (id == 1)
                {
                    fprintf(dst, "-%s", c2);

                    //strcat(mod_chg,"c");
                    strcat(mod_chg, t);
                    strcat(mod_chg, "\t-");
                    strcat(mod_chg, c2);
                    send(clientSocket, mod_chg, strlen(mod_chg), 0);

                    memset(mod_chg, 0, sizeof(mod_chg));
                    free(timep);
                }
                if (id == 2)
                {
                    fprintf(dst, "+%s", c2);

                    //strcat(mod_chg,"c");
                    strcat(mod_chg, t);
                    strcat(mod_chg, "\t+");
                    strcat(mod_chg, c2);
                    send(clientSocket, mod_chg, strlen(mod_chg), 0);

                    memset(mod_chg, 0, sizeof(mod_chg));
                    free(timep);
                }
                fclose(dst);
            }
        }
        fclose(fp2);
        j2 = 0;
        memset(c1, 0, sizeof(c1));
        memset(c2, 0, sizeof(c2));

        j1 = j2 = 0;

        fp1 = fopen("temp1.txt", "r");
        while (fgets(c1, sizeof(c1), fp1) != NULL)
        {
            j1++;
            if (j1 == i1)
                strcat(c1, " \n");

            fp2 = fopen("temp2.txt", "r");
            while (fgets(c2, sizeof(c2), fp2) != NULL)
            {
                flag = 0;
                for (i = 0; c1[i] != '\n' && c2[i] != '\n'; i++)
                    if (c1[i] != c2[i])
                    {
                        flag = 1;
                        break;
                    }

                if (c1[i] != '\n' || c2[i] != '\n')
                    flag = 1;
                if (flag == 0)
                    break;
            }
            fclose(fp2);
            j2 = 0;
            if (flag)
            {
                if (id == 1)
                    printf("+");
                if (id == 2)
                    printf("-");
                printf("%s", c1);

                dst = fopen("module.txt", "a+");

                //time
                timep = malloc(sizeof(*timep));
                time(timep);
                t = ctime(timep);
                t[strlen(t) - 1] = '\0';
                fprintf(dst, "%s\t", t);

                if (id == 1)
                {
                    fprintf(dst, "+%s", c1);

                    //strcat(mod_chg,"c");
                    strcat(mod_chg, t);
                    strcat(mod_chg, "\t+");
                    strcat(mod_chg, c1);
                    send(clientSocket, mod_chg, strlen(mod_chg), 0);

                    memset(mod_chg, 0, sizeof(mod_chg));
                    free(timep);
                }
                if (id == 2)
                {
                    fprintf(dst, "-%s", c1);

                    //strcat(mod_chg,"c");
                    strcat(mod_chg, t);
                    strcat(mod_chg, "\t-");
                    strcat(mod_chg, c1);
                    send(clientSocket, mod_chg, strlen(mod_chg), 0);

                    memset(mod_chg, 0, sizeof(mod_chg));
                    free(timep);
                }
                fclose(dst);
            }
        }
        fclose(fp1);
        j1 = 0;
        memset(c1, 0, sizeof(c1));
        memset(c2, 0, sizeof(c2));

        send(clientSocket, "@\n", 2, 0);

        if (id == 1)
        {
            dst = fopen("temp2.txt", "w");
            fclose(dst);
        }
        if (id == 2)
        {
            dst = fopen("temp1.txt", "w");
            fclose(dst);
        }
        //结果存入历史文件
        dst = fopen("module.txt", "a+");
        if (dst != NULL)
            while (fgets(c1, sizeof(c1), dst) != NULL)
            {
                his = fopen("module_history.txt", "a+");
                fprintf(his, "%s", c1);
                fclose(his);

                /*strcat(mod_his,"h");
                strcat(mod_his, c1);
                send(clientSocket, mod_his, strlen(mod_his), 0);

                memset(mod_his, 0, sizeof(mod_his));*/
            }
        memset(c1, 0, sizeof(c1));
        fclose(dst);

        sleep(5);
    }

    close(clientSocket);
    return 0;
}
