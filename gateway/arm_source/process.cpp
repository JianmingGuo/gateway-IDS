#include<fstream>
#include<iostream>
using namespace std;
#include<ctime>
#include<stdio.h>
#include <unistd.h>
#include <string.h>
#include<dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<stdlib.h>
#define interval 90
//一次发送的版本

bool IsDigit(string str)
{
for(int i=0;i<str.size();i++)
{
if ((str.at(i)>'9') || (str.at(i)<'0') )
{
return false;
}
}
return true;
}
 void get_status_uid(char *path1,int *uid){
    FILE * net_dev_file;	//文件指针
    char buffer[1024];	//文件中的内容暂存在字符缓冲区里
    size_t bytes_read;	//实际读取的内容大小 
    char * match;	 //用以保存所匹配字符串及之后的内容
    char path2[10]="/status";
    char path3[30];
     sprintf(path3,"%s%s",path1,path2);
    if ( (net_dev_file=fopen(path3, "r")) == NULL ) //打开文件/proc/pid/status，我们要读取的数据就是它
    {
        printf("open file  error!%s\n",path3);
        exit(EXIT_FAILURE);
    }
    bytes_read = fread(buffer, 1, sizeof(buffer), net_dev_file);//将文件中的1024个字符大小的数据保存到buffer里
    fclose(net_dev_file); //关闭文件
    if ( bytes_read == 0 )//如果文件内容大小为０，没有数据则退出
    {
        exit(EXIT_FAILURE);
    }
    buffer[bytes_read] = '\0';
    match = strstr(buffer, "Uid:");//匹配Uid:第一次出现的位置，返回值为第一次出现的位置的地址
    if ( match == NULL )
    {
        printf("no Uid: keyword to find!\n");
        exit(EXIT_FAILURE);
    }
    sscanf(match, "Uid:%d",uid );//从字符缓冲里读取数据，这个值就是当前的流量
 }
 





void  get_exe_path(char*path1,char*buf)
{
   // static char buf[PATH_MAX];//定义最大路径长度
    char path2[10]="/exe";
    char path3[30];
    sprintf(path3,"%s%s",path1,path2);
    int rslt = readlink(path3, buf, 1024);
    if (rslt < 0 || rslt >= 1024)
    {
        return;
    }
    buf[rslt] = '\0';
    return ;
}




void get_status_name(char *path1,char * name){
    FILE * net_dev_file;	//文件指针
    char buffer[1024];	//文件中的内容暂存在字符缓冲区里
    size_t bytes_read;	//实际读取的内容大小 
    char * match;	 //用以保存所匹配字符串及之后的内容
    char path2[10]="/status";
    char path3[30];
     sprintf(path3,"%s%s",path1,path2);
    if ( (net_dev_file=fopen(path3, "r")) == NULL ) //打开文件/proc/pid/status，我们要读取的数据就是它
    {
        printf("open file  error!%s\n",path3);
        exit(EXIT_FAILURE);
    }
    bytes_read = fread(buffer, 1, sizeof(buffer), net_dev_file);//将文件中的1024个字符大小的数据保存到buffer里
    fclose(net_dev_file); //关闭文件
    if ( bytes_read == 0 )//如果文件内容大小为０，没有数据则退出
    {
        exit(EXIT_FAILURE);
    }
    buffer[bytes_read] = '\0';
    match = strstr(buffer, "Name:");//匹配Name:第一次出现的位置，返回值为第一次出现的位置的地址
    if ( match == NULL )
    {
        printf("no Name: keyword to find!\n");
        exit(EXIT_FAILURE);
    }
    sscanf(match, "Name:%s",name );//从字符缓冲里读取数据，这个值就是当前的流量
}

int main(int argc, char* argv[]){
    int sockfd;
    char probuf[65536];
    struct sockaddr_in servaddr;
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
     if(send(sockfd,"2",1,0)<0)
        {
            printf("send msg error :%s(errno:%d)\n",strerror(errno),errno);
            exit(0);
        }

  while(true){
   

        probuf[0]='\0';
        DIR* dir_ptr;  //the directory
	    struct dirent* direntp; //each entry
        char filename[6]="/proc";
        char path1[512];
        char path[10]="/proc/";
        char name[30];
        char buf[1024];
        int uid;
        dir_ptr = opendir(filename);
        while((direntp = readdir(dir_ptr)) != NULL){
       if(IsDigit(direntp->d_name))
        {
          buf[0]='\0';
          sprintf(path1,"%s%s",path,direntp->d_name);
          get_status_uid(path1,&uid);
          get_status_name(path1,name);
          get_exe_path(path1,buf);
          if(strlen(buf)==0)
          strcpy(buf,"/");
          sprintf(probuf,"%s%s\t%s\t%d\t%s\n",probuf,name,direntp->d_name,uid,buf);
        }
        }
        probuf[strlen(probuf)-1]='\0';
        if(send(sockfd,probuf,strlen(probuf),0)<0)
        {
            printf("send msg error :%s(errno:%d)\n",strerror(errno),errno);
            exit(0);
        }
        printf("%s\n",probuf);
    sleep(interval);
    
   }
   close(sockfd);
   return 0;
}