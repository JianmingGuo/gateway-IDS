#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;


int main()
{
    int k=0;
    string a;
    string id[50];
    int tmp1,tmp2;
    int idnum=1;

    bool c[50][50];
     for(int i=0;i<50;i++)  //创建0c
  {
      for(int j=0;j<50;j++)
      {
          c[i][j]=0;
      }

  }



    ifstream fin("normal_run_data.txt");  //读取

    if(!fin){
        printf("The file is not exist!");
        return -1;
    }



while(a!="ID:")
  {
      fin>>a;
  }
  fin>>a;
  id[0]=a;
  tmp2=0;     //存入第一个id



for(int j=0;j<900000;j++)//循环多次
{

    while(a!="ID:")  //找到id:后的数字
  {
      fin>>a;
  }
  fin>>a;

      for(k=0;k<50;k++)   //在id数组中找到相应位置
      {
         if( a==id[k])
         break;
      }

      if(k==50)       //若id数组中没有，则添加
      {
          id[idnum]=a;
          k=idnum;
          idnum++;
      }

      tmp1=tmp2;           //修改转移矩阵
      tmp2=k;
      c[tmp1][tmp2]=1;


}

for(int i=0;i<idnum;i++)//输出id
{
    cout<<id[i]<<endl;
}

  for(int i=0;i<idnum;i++)  //输出c
  {
      for(int j=0;j<idnum;j++)
      {
          cout<<c[i][j]<<' ';
      }
      cout<<endl;
  }


    fin.close();





    return 0;
}
