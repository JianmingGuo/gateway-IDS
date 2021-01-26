#include <fstream>
#include<iostream>
#include<ctime>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;
// vehicle

#define interval 1

long int getCurrentDownloadBytes(long int *save_rate);
long int getCurrentUploadBytes(long int * save_rate);

int main(int argc, char* argv[]){


    int sockfd;
    char ratebuf[128];
    struct sockaddr_in servaddr;


    long int start_download_rates;
    long int end_download_rates;
    long int start_upload_rates;
    long int end_upload_rates;
    int i=0;


    if(argc!=2)
    {
        printf("usage:./client <ipaddress>\n");
        exit(0);
    }

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
     if(send(sockfd,"1",1,0)<0)
        {
            printf("send msg error :%s(errno:%d)\n",strerror(errno),errno);
            exit(0);
        }


        while(true){
        getCurrentDownloadBytes(&start_download_rates);
        getCurrentUploadBytes(&start_upload_rates);
        sleep(interval); 
        getCurrentDownloadBytes(&end_download_rates);
        getCurrentUploadBytes(&end_upload_rates);
        sprintf(ratebuf,"%ld\n%ld",end_download_rates-start_download_rates,end_upload_rates-start_upload_rates);
        if(send(sockfd,ratebuf,strlen(ratebuf),0)<0)
        {
            printf("send msg error :%s(errno:%d)\n",strerror(errno),errno);
            exit(0);
        }

        }
        close(sockfd);
    return 0;
}


long int getCurrentDownloadBytes(long int * save_rate)
{
    FILE * net_dev_file;	//文件指针
    char buffer[1024];	//文件中的内容暂存在字符缓冲区里
    size_t bytes_read;	//实际读取的内容大小 
    char * match;	 //用以保存所匹配字符串及之后的内容
    if ( (net_dev_file=fopen("/proc/net/dev", "r")) == NULL ) //打开文件/pro/net/dev/，我们要读取的数据就是它啦
    {
        printf("open file /proc/net/dev/ error!\n");
        exit(EXIT_FAILURE);
    }
    bytes_read = fread(buffer, 1, sizeof(buffer), net_dev_file);//将文件中的1024个字符大小的数据保存到buffer里
    fclose(net_dev_file); //关闭文件
    if ( bytes_read == 0 )//如果文件内容大小为０，没有数据则退出
    {
        exit(EXIT_FAILURE);
    }
    buffer[bytes_read] = '\0';
    match = strstr(buffer, "eth0:");//匹配wlp0s20f3:第一次出现的位置，返回值为第一次出现的位置的地址
    if ( match == NULL )
    {
        printf("no eth0: keyword to find!\n");
        exit(EXIT_FAILURE);
    }
    sscanf(match, "eth0:%ld", save_rate);//从字符缓冲里读取数据，这个值就是当前的流量
    return *save_rate;
}

long int getCurrentUploadBytes(long int * save_rate)
{
    FILE * net_dev_file;	//文件指针
    char buffer[1024];	//文件中的内容暂存在字符缓冲区里
    size_t bytes_read;	//实际读取的内容大小 
    char * match;	 //用以保存所匹配字符串及之后的内容
    if ( (net_dev_file=fopen("/proc/net/dev", "r")) == NULL ) //打开文件/pro/net/dev/，我们要读取的数据就是它啦
    {
        printf("open file /proc/net/dev/ error!\n");
        exit(EXIT_FAILURE);
    }
    bytes_read = fread(buffer, 1, sizeof(buffer), net_dev_file);//将文件中的1024个字符大小的数据保存到buffer里
    fclose(net_dev_file); //关闭文件
    if ( bytes_read == 0 )//如果文件内容大小为０，没有数据则退出
    {
        exit(EXIT_FAILURE);
    }
    buffer[bytes_read] = '\0';
    match = strstr(buffer, "eth0:");//匹配第一次出现的位置，返回值为第一次出现的位置的地址
    if ( match == NULL )
    {
        printf("no eth0: keyword to find!\n");
        exit(EXIT_FAILURE);
    }
    sscanf(match, "eth0:%*d%*d%*d%*d%*d%*d%*d%*d%ld", save_rate);//从字符缓冲里读取数据，这个值就是当前的流量
    return *save_rate;
}