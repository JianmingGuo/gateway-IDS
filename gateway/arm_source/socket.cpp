#include<fstream>
#include<iostream>
using namespace std;
#include<ctime>
#include <unistd.h>
#include <string.h>
#include<dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
void get_status_name(char *path1,char * name){
    FILE * net_dev_file;	//文件指针
    char buffer[1024];	//文件中的内容暂存在字符缓冲区里
    size_t bytes_read;	//实际读取的内容大小 
    char * match;	 //用以保存所匹配字符串及之后的内容
    char path2[10]="/status";
    char path3[30];
     sprintf(path3,"%s%s",path1,path2);
    if ( (net_dev_file=fopen(path3, "r")) == NULL ) 
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
    sscanf(match, "Name:%s",name );//从字符缓冲里读取数据
}


char * find_process(long int inode,char* pstat)
{
        DIR* dir_ptr;  //the directory
	    struct dirent* direntp; //each entry
         DIR* dir_ptr1;  //the directory
	    struct dirent* direntp1; //each entry
        char filename[6]="/proc";
        dir_ptr = opendir(filename);
        char path1[300];
        char path[10]="/proc/";
        char fd[10]="/fd";
        char path2[400];
        char buf[1000];
        int rst;
        while((direntp = readdir(dir_ptr)) != NULL)
       if(IsDigit(direntp->d_name))
        {
            sprintf(path1,"%s%s%s",path,direntp->d_name,fd);
            dir_ptr1 = opendir(path1);
            while((direntp1 = readdir(dir_ptr1)) != NULL)
            {
                //cout<<path1<<direntp1->d_name<<endl;
                sprintf(path2,"%s%s%s",path1,"/",direntp1->d_name);
                rst=readlink(path2,buf,1000);
                char inodes[10];
                sprintf(inodes,"%ld",inode);
                if(strstr(buf,inodes)!=NULL){
                pstat= direntp->d_name;
                sprintf(path2,"%s%s",path,direntp->d_name);
                char pname[100]="";
                get_status_name(path2,pname);
                strcat(pstat,"\t");
                strcat(pstat,pname);
                closedir(dir_ptr);
                return pstat;
                }
            }
            closedir(dir_ptr1);
        }
}


int main(int argc, char* argv[])
{
    
    

    int sockfd;
    char socbuf[2048];
    struct sockaddr_in servaddr;

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
     if(send(sockfd,"3",1,0)<0)
        {
            printf("send msg error :%s(errno:%d)\n",strerror(errno),errno);
            exit(0);
        }


    
	string filename;
	string line;
    int*  stat;
    stat=new int;
    unsigned  int  * address;
    address=new unsigned  int ;
    long int * inode;
    inode= new long int;
    char error[6]="error";
    char *pstat=error;
    char ip[256];
    while(true){
        ifstream tcp("/proc/net/tcp"),tcp6("/proc/net/tcp6"),udp("/proc/net/udp"),udp6("/proc/net/udp6"),raw6("/proc/net/raw6");
         socbuf[0]='\0';
    if(tcp) // 有该文件
	{
        
		getline (tcp, line);
        while (getline (tcp, line)) // line中不包括每行的换行符
		{ 
			char*p = strdup (line.c_str());
            sscanf(p,"%*s%*s%*s%x",stat);
            if(*stat==10)
            {
                sscanf(p,"%*s%x",address);
                sprintf(ip, "%d.%d.%d.%d",
                *address    & 0xFF,
                (*address >> 8) & 0xFF,
                (*address >> 16) & 0xFF,
                (*address >>24)& 0xFF);
                 sscanf(p,"%*s%*9s%4x",address);

                 sscanf(p,"%*s%*s%*s%*s%*s%*s%*s%*s%*s%ld",inode);

                 pstat=find_process(*inode,pstat);

                sprintf(socbuf,"%stcp\t%s:%u\t%s\n",socbuf,ip,*address,pstat);

            }
		}
	}
	else // 没有该文件
	{
		cout <<"no such file" << endl;
	}


    if(tcp6) // 有该文件
	{
		getline (tcp6, line);
        while (getline (tcp6, line)) // line中不包括每行的换行符
		{ 
			char*p = strdup (line.c_str());
            sscanf(p,"%*s%*s%*s%x",stat);
            if(*stat==10)
            {
                sscanf(p,"%*s%8x",address);
                sprintf(ip, "%d.%d.%d.%d",
                *address      & 0xFF,
                (*address >> 8) & 0xFF,
                (*address >> 16) & 0xFF,
                (*address >> 24)& 0xFF);
                sscanf(p,"%*s%*8x%8x",address);
                sprintf(ip, "%s.%d.%d.%d.%d",
                ip,
                *address      & 0xFF,
                (*address >> 8) & 0xFF,
                (*address >> 16) & 0xFF,
                (*address >> 24)& 0xFF);
                sscanf(p,"%*s%*16x%8x",address);
                sprintf(ip, "%s.%d.%d.%d.%d",
                ip,
                *address      & 0xFF,
                (*address >> 8) & 0xFF,
                (*address >> 16) & 0xFF,
                (*address >> 24)& 0xFF);
                sscanf(p,"%*s%*24x%8x",address);
                sprintf(ip, "%s.%d.%d.%d.%d",
                ip,
                *address      & 0xFF,
                (*address >> 8) & 0xFF,
                (*address >> 16) & 0xFF,
                (*address >> 24)& 0xFF);
                 sscanf(p,"%*s%*9s%4x",address);

                 sscanf(p,"%*s%*s%*s%*s%*s%*s%*s%*s%*s%ld",inode);

                 pstat=find_process(*inode,pstat);

                sprintf(socbuf,"%stcp6\t%s:%u\t%s\n",socbuf,ip,*address,pstat);
                
            }
		}
	}
	else // 没有该文件
	{
		cout <<"no such file" << endl;
	}


    if(udp) // 有该文件
	{
		getline (udp, line);
        while (getline (udp, line)) // line中不包括每行的换行符
		{ 
			char*p = strdup (line.c_str());
           
                sscanf(p,"%*s%x",address);
                sprintf(ip, "%d.%d.%d.%d",
                *address    & 0xFF,
                (*address >> 8) & 0xFF,
                (*address >> 16) & 0xFF,
                (*address >>24)& 0xFF);
                 sscanf(p,"%*s%*9s%4x",address);

                 sscanf(p,"%*s%*s%*s%*s%*s%*s%*s%*s%*s%ld",inode);

                 pstat=find_process(*inode,pstat);

                sprintf(socbuf,"%sudp\t%s:%u\t%s\n",socbuf,ip,*address,pstat);
		}
	}
	else // 没有该文件
	{
		cout <<"no such file" << endl;
	}


    if(udp6) // 有该文件
	{
		getline (udp6, line);
        while (getline (udp6, line)) // line中不包括每行的换行符
		{ 
			char*p = strdup (line.c_str());
                            sscanf(p,"%*s%8x",address);
                sprintf(ip, "%d.%d.%d.%d",
                *address      & 0xFF,
                (*address >> 8) & 0xFF,
                (*address >> 16) & 0xFF,
                (*address >> 24)& 0xFF);
                sscanf(p,"%*s%*8x%8x",address);
                sprintf(ip, "%s.%d.%d.%d.%d",
                ip,
                *address      & 0xFF,
                (*address >> 8) & 0xFF,
                (*address >> 16) & 0xFF,
                (*address >> 24)& 0xFF);
                sscanf(p,"%*s%*16x%8x",address);
                sprintf(ip, "%s.%d.%d.%d.%d",
                ip,
                *address      & 0xFF,
                (*address >> 8) & 0xFF,
                (*address >> 16) & 0xFF,
                (*address >> 24)& 0xFF);
                sscanf(p,"%*s%*24x%8x",address);
                sprintf(ip, "%s.%d.%d.%d.%d",
                ip,
                *address      & 0xFF,
                (*address >> 8) & 0xFF,
                (*address >> 16) & 0xFF,
                (*address >> 24)& 0xFF);
                 sscanf(p,"%*s%*9s%4x",address);

                 sscanf(p,"%*s%*s%*s%*s%*s%*s%*s%*s%*s%ld",inode);

                 pstat=find_process(*inode,pstat);

                sprintf(socbuf,"%sudp6\t%s:%u\t%s\n",socbuf,ip,*address,pstat);
		}
	}
	else // 没有该文件
	{
		cout <<"no such file" << endl;
	}
    
    if(raw6) // 有该文件
	{
		getline (raw6, line);
        while (getline (raw6, line)) // line中不包括每行的换行符
		{ 
			char*p = strdup (line.c_str());
                sscanf(p,"%*s%8x",address);
                sprintf(ip, "%d.%d.%d.%d",
                *address      & 0xFF,
                (*address >> 8) & 0xFF,
                (*address >> 16) & 0xFF,
                (*address >> 24)& 0xFF);
                sscanf(p,"%*s%*8x%8x",address);
                sprintf(ip, "%s.%d.%d.%d.%d",
                ip,
                *address      & 0xFF,
                (*address >> 8) & 0xFF,
                (*address >> 16) & 0xFF,
                (*address >> 24)& 0xFF);
                sscanf(p,"%*s%*16x%8x",address);
                sprintf(ip, "%s.%d.%d.%d.%d",
                ip,
                *address      & 0xFF,
                (*address >> 8) & 0xFF,
                (*address >> 16) & 0xFF,
                (*address >> 24)& 0xFF);
                sscanf(p,"%*s%*24x%8x",address);
                sprintf(ip, "%s.%d.%d.%d.%d",
                ip,
                *address      & 0xFF,
                (*address >> 8) & 0xFF,
                (*address >> 16) & 0xFF,
                (*address >> 24)& 0xFF);
                 sscanf(p,"%*s%*9s%4x",address);

                 sscanf(p,"%*s%*s%*s%*s%*s%*s%*s%*s%*s%ld",inode);

                 pstat=find_process(*inode,pstat);

                sprintf(socbuf,"%sraw6\t%s:%u\t%s\n",socbuf,ip,*address,pstat);
                
		}
	}
	else // 没有该文件
	{
		cout <<"no such file" << endl;
	}
    if(send(sockfd,socbuf,strlen(socbuf),0)<0)
        {
            printf("send msg error :%s(errno:%d)\n",strerror(errno),errno);
            exit(0);
        }
        printf("%s\n",socbuf);
    sleep(40);
    }
    close(sockfd);
    return 0;
}