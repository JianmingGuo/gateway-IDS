

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
#define DELETE_RATE "delete  from rates"
#define DELETE_SOC "delete  from socket"
#define DELETE_PRO "delete  from process"
#define DELETE_DE_PRO "delete  from deviant_process"
#define DELETE_DE_SOC "delete  from deviant_socket"
#define DEFAULT_PORT 8000
#define MAXLIN 4096
 //database method  detection
int main(int argc,char **argv)
{
    int socket_fd,connect_fd;
    struct sockaddr_in servaddr;
    char buff[128], pro_buff[65536] ,soc_buff[2048],a[100000];
    char rate_ist[200],dl_buf[15],ul_buf[15];
    char soc_ist[2000],pro_ist[200000],pro_ist2[300],soc_ist2[300];
    char * soc_p[4],*pro_p[5];
    int i=0;
    char *soc_buff2, *out, *inner,*pro_buff2;
    FILE *PRO,*SOC,*RATE;
    int n;
    MYSQL *siov;
    MYSQL_RES *result;
    strcpy(pro_ist,"insert into process(name,pid,uid,path) values");
    strcpy(soc_ist,"insert into socket(protocol,socket,process_id,process_name) values");




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
                printf("%s\n",buff);
                sprintf(rate_ist,"insert into rates(DL_rates,UL_rates) values(%s,%s)",dl_buf,ul_buf);
              if (mysql_query(siov,DELETE_RATE))
                {
                       printf("Error delete rates:%s\n",mysql_error(siov));
                }
                if (mysql_query(siov,rate_ist))
                {
                       printf("Error insert rates:%s\n",mysql_error(siov));
                }
                sprintf(rate_ist,"select * from PT_rates where %s>(select max(DL_rates) from PT_rates) OR  %s>(select max(UL_rates) from PT_rates)",dl_buf,ul_buf);
                if (mysql_query(siov,rate_ist))
                {
                       printf("Error making query rate search:%s\n",mysql_error(siov));
                }
                result=mysql_store_result(siov);
                if(mysql_num_rows(result)!=0){
                sprintf(rate_ist,"insert into deviant_rates(DL_rates,UL_rates,time) values(%s,%s,now())",dl_buf,ul_buf);
                if (mysql_query(siov,rate_ist))
                {
                       printf("Error insert deviant rates:%s\n",mysql_error(siov));
                }
                }
                mysql_free_result(result);
                
               }
                close(connect_fd);
                exit(0);
                break;
            case '2':
                while(1){
                n=recv(connect_fd,pro_buff,65536,0);
		int m=recv(connect_fd,pro_buff+n,65536,0);
                pro_buff[n+m]='\0';
                pro_buff2=pro_buff;
                printf("%s\n",pro_buff);
                if (mysql_query(siov,DELETE_DE_PRO))
                {
                       printf("Error making query:%s\n",mysql_error(siov));
                }
                while((pro_p[i]=strtok_r(pro_buff2,"\n",&out))!=NULL){
                    pro_buff2=pro_p[i];
                    while((pro_p[i]=strtok_r(pro_buff2,"\t",&inner))!=NULL){
                        i++;
                        pro_buff2=NULL;
                    }
                    pro_buff2=NULL;
                    i=0;
                    sprintf(pro_ist,"%s(\"%s\",%s,%s,\"%s\"),",pro_ist,pro_p[0],pro_p[1],pro_p[2],pro_p[3]);
                    sprintf(pro_ist2,"select * from PT_process where name=\"%s\" and uid=\"%s\" and path= \"%s\"",pro_p[0],pro_p[2],pro_p[3]);
                    if (mysql_query(siov,pro_ist2))
                {
                       printf("Error making query pro search:%s\n",mysql_error(siov));
                }
                result=mysql_store_result(siov);
                if(mysql_num_rows(result)<1){
                sprintf(pro_ist2,"insert into deviant_process(name,pid,uid,path,time) values(\"%s\",%s,%s,\"%s\",now())",pro_p[0],pro_p[1],pro_p[2],pro_p[3]);
                if (mysql_query(siov,pro_ist2))
                {
                       printf("Error making query: deviant pro insert %s\n%s",mysql_error(siov),pro_p[3]);
                }
                }
                mysql_free_result(result);
                }
              if (mysql_query(siov,DELETE_PRO))
                {
                       printf("Error making query:%s\n",mysql_error(siov));
                }
                pro_ist[strlen(pro_ist)-1]='\0';
                if (mysql_query(siov,pro_ist))
                {
                       printf("Error insert pro :%s\n",mysql_error(siov));
                }
		    strcpy(pro_ist,"insert into process(name,pid,uid,path) values");
                }
                close(connect_fd);
                exit(0);
                case '3':
                while(1){
                n=recv(connect_fd,soc_buff,2048,0);
                soc_buff[n]='\0';
                soc_buff2=soc_buff;
                if (mysql_query(siov,DELETE_DE_SOC))
                {
                       printf("Error making query:%s\n",mysql_error(siov));
                }
                while((soc_p[i]=strtok_r(soc_buff2,"\n",&out))!=NULL){
                    soc_buff2=soc_p[i];
                    while((soc_p[i]=strtok_r(soc_buff2,"\t",&inner))!=NULL){
                        i++;
                        soc_buff2=NULL;
                    }
                    soc_buff2=NULL;
                    i=0;
                    sprintf(soc_ist,"%s(\"%s\",\"%s\",%s,\"%s\"),",soc_ist,soc_p[0],soc_p[1],soc_p[2],soc_p[3]);
                    sprintf(soc_ist2,"select * from PT_socket where protocol=\"%s\" and socket=\"%s\" and process_name= \"%s\"",soc_p[0],soc_p[1],soc_p[3]);
                if (mysql_query(siov,soc_ist2))
                {
                       printf("Error making query soc search:%s\n",mysql_error(siov));
                }
                result=mysql_store_result(siov);
                if(mysql_num_rows(result)<1){
                sprintf(soc_ist2,"insert into deviant_socket(protocol,socket,process_id,process_name,time) values(\"%s\",\"%s\",\"%s\",\"%s\",now())",soc_p[0],soc_p[1],soc_p[2],soc_p[3]);
                if (mysql_query(siov,soc_ist2))
                {
                       printf("Error making query: soc insert %s\n",mysql_error(siov));
                }
                }
                mysql_free_result(result);
                }
                soc_ist[strlen(soc_ist)-1]='\0';
              if (mysql_query(siov,DELETE_SOC))
                {
                       printf("Error making query:%s\n",mysql_error(siov));
                }
                if (mysql_query(siov,soc_ist))
                {
                       printf("Error making query:%s\n",mysql_error(siov));
                }
    		strcpy(soc_ist,"insert into socket(protocol,socket,process_id,process_name) values");
                }
                close(connect_fd);
                exit(0);
                break;
                default:
                break;
            }
		while(1)
		recv(connect_fd,a,100000,0);
        }
    close(connect_fd);

    }
    close(socket_fd);
    mysql_close(siov); 
    return 0;
}
