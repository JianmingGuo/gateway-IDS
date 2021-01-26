#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char* argv[]){
 
     int socket_fd1,connect_fd;
    struct sockaddr_in servaddr1;
       //初始化
    if((socket_fd1=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        printf("create socket error:%s(errno :%d)\n",strerror(errno),errno);
        exit(0);
    }
    memset(&servaddr1,0,sizeof(servaddr1));
    servaddr1.sin_family = AF_INET;
    servaddr1.sin_addr.s_addr=htonl(INADDR_ANY);//IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址
    servaddr1.sin_port=htons(3000);
    //设置的端口为DEFAULT_PORT
    //将本地地址绑定到所创建的套接字上
    if(bind(socket_fd1,(struct sockaddr*)&servaddr1,sizeof(servaddr1))==-1)
    {
        printf("bind socket error:%s(errno:%d)\n",strerror(errno),errno);
        exit(0);
    }
    //开始监听是否有客户端连接，第二个参数是最大监听数
    if(listen(socket_fd1,3)==-1)
    {
        printf("listen socket error:%s(errno:%d)\n",strerror(errno),errno);
        exit(0);
    }
 
        int sockfd;
        struct sockaddr_in servaddr;
        char a[10000];
    if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)//(协议族/域，协议类型，协议编号)
    {
        printf("create socket error:%s(error:%d)\n",strerror(errno),errno);
        exit(0);
    }
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;/* 该属性表示接收本机或其它机器传输 */
    servaddr.sin_port=htons(8000);/* 端口号 */
    if(inet_pton(AF_INET,argv[1],&servaddr.sin_addr)<=0)
    {
        printf("inet_pton error for %s\n",argv[1]);
        exit(0);
    }
    if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
    {
        printf("connect error:%s(errno:%d)\n",strerror(errno),errno);
        exit(0);
    }

    for(int i=0; i<10000; i++)
    {
        a[i]='a';
    }
    while(1){
    if(send(sockfd,a,strlen(a),0)<0)
        {
            printf("send msg error :%s(errno:%d)\n",strerror(errno),errno);
            exit(0);
        }
        sleep(0.5);
    }

}