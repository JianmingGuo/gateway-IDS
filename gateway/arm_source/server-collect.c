

#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <mysql/mysql.h>

#define HOST "39.100.88.210"
#define USER "siov"
#define DEFAULT_PORT 8000
#define MAXLIN 4096
 //database method collection
int main(int argc,char **argv)
{
    int socket_fd,connect_fd;
    struct sockaddr_in servaddr;
    char buff[128], pro_buff[65536] ,soc_buff[2048];
    char rate_ist[100],dl_buf[15],ul_buf[15];
    char soc_ist[200],pro_ist[200];
    char * soc_p[5],*pro_p[5];
    MYSQL_RES *result;
    int i=0;
    char *soc_buff2, *out=NULL, *inner=NULL,*pro_buff2;
    FILE *PRO,*SOC,*RATE;
    int n;
    MYSQL *siov;
    strcpy(pro_ist,"insert into process(name, pid,uid,path) values(\"\",0,0,\"\")");



    //初始化
    if((socket_fd=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        printf("create socket error:%s(errno :%d)\n",strerror(errno),errno);
        exit(0);
    }
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);//IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址
    servaddr.sin_port=htons(DEFAULT_PORT);
    //设置的端口为DEFAULT_PORT
    //将本地地址绑定到所创建的套接字上
    if(bind(socket_fd,(struct sockaddr*)&servaddr,sizeof(servaddr))==-1)
    {
        printf("bind socket error:%s(errno:%d)\n",strerror(errno),errno);
        exit(0);
    }
    //开始监听是否有客户端连接，第二个参数是最大监听数
    if(listen(socket_fd,3)==-1)
    {
        printf("listen socket error:%s(errno:%d)\n",strerror(errno),errno);
        exit(0);
        
    }
    printf("======waiting for client's request=====\n");

    while(1)
    {
        siov = mysql_init(NULL);
if(siov == NULL)
{
       printf("init  mysql object failed!\n");
}
else
{
       printf("init mysql object successed!\n");
}
 if(mysql_real_connect(siov,HOST,USER,USER,USER,0,NULL,0))
{
         printf("connect successed!\n");
}
else
{
         printf("connect failed!\n");
}
        if((connect_fd=accept(socket_fd,(struct sockaddr*)NULL,NULL))==-1){
            printf("accept socket error :%s(errno:%d)\n",strerror(errno),errno);
            continue;
        }
        recv(connect_fd,buff,128,0);
        if(!fork()){
            switch (buff[0])
            {
            case '1':
               while(1){
                n=recv(connect_fd,buff,128,0);
                sscanf(buff,"%s%s",dl_buf,ul_buf);
                buff[n]='\0';
                sprintf(rate_ist,"insert into PT_rates(DL_rates,UL_rates,time) values(%s,%s,now())",dl_buf,ul_buf);
                if (mysql_query(siov,rate_ist))
                {
                       printf("Error making query rate :%s\n",mysql_error(siov));
                }

           /*     RATE=fopen("rate2.txt","w");
                fprintf(RATE,"%s",buff);
                fclose(RATE);*/
                }
                close(connect_fd);
                exit(0);
                break;
            case '2':
                while(1){
                n=recv(connect_fd,pro_buff,65536,0);
		int m =recv(connect_fd,pro_buff+n,65536,0);
                pro_buff[n+m]='\0';
                printf("%s\n",pro_buff);
                printf("%d %d\n",n,m);
                pro_buff2=pro_buff;
                while((pro_p[i]=strtok_r(pro_buff2,"\n",&out))!=NULL){
                    pro_buff2=pro_p[i];

                    while((pro_p[i]=strtok_r(pro_buff2,"\t",&inner))!=NULL){

                        i++;
                        pro_buff2=NULL;
                    }
                    pro_buff2=NULL;
                    i=0;
                    sprintf(pro_ist,"select * from PT_process where name=\"%s\" and uid=\"%s\" and path= \"%s\"",pro_p[0],pro_p[2],pro_p[3]);

                if (mysql_query(siov,pro_ist))
                {
                       printf("Error making query pro search:%s\n",mysql_error(siov));
                }
                result=mysql_store_result(siov);
                if(mysql_num_rows(result)<1){
                sprintf(pro_ist,"insert into PT_process(name,pid,uid,path,time) values(\"%s\",%s,%s,\"%s\",now())",pro_p[0],pro_p[1],pro_p[2],pro_p[3]);
                if (mysql_query(siov,pro_ist))
                {
                       printf("Error making query: pro insert %s\n%s",mysql_error(siov),pro_p[3]);
                }
                }
                mysql_free_result(result);
                }
                /*PRO=fopen("process2.txt","w");
                fprintf(PRO,"%s",pro_buff);
                fclose(PRO);*/
                }
                close(connect_fd);
                exit(0);
                case '3':
                while(1){
                n=recv(connect_fd,soc_buff,2048,0);
                soc_buff[n]='\0';
                soc_buff2=soc_buff;
                while((soc_p[i]=strtok_r(soc_buff2,"\n",&out))!=NULL){
                    soc_buff2=soc_p[i];
                    while((soc_p[i]=strtok_r(soc_buff2,"\t",&inner))!=NULL){
                        i++;
                        soc_buff2=NULL;
                    }
                    soc_buff2=NULL;
                    i=0;
                sprintf(soc_ist,"select * from PT_socket where protocol=\"%s\" and socket=\"%s\" and process_name= \"%s\"",soc_p[0],soc_p[1],soc_p[3]);
                if (mysql_query(siov,soc_ist))
                {
                       printf("Error making query soc search:%s\n",mysql_error(siov));
                }
                result=mysql_store_result(siov);
                if(mysql_num_rows(result)<1){
                sprintf(soc_ist,"insert into PT_socket(protocol,socket,process_id,process_name,time) values(\"%s\",\"%s\",\"%s\",\"%s\",now())",soc_p[0],soc_p[1],soc_p[2],soc_p[3]);
                if (mysql_query(siov,soc_ist))
                {
                       printf("Error making query: soc insert %s\n",mysql_error(siov));
                }
                }
                mysql_free_result(result);
                }
                /*SOC=fopen("socket2.txt","w");
                fprintf(SOC,"%s",soc_buff);
                fclose(SOC);*/
                }
                close(connect_fd);
                exit(0);
                break;
                default:
                break;
            }
        }
    close(connect_fd);

    }
    close(socket_fd);
    mysql_close(siov); 
    return 0;
}
