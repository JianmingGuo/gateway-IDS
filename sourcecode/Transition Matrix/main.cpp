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
     for(int i=0;i<50;i++)  //����0c
  {
      for(int j=0;j<50;j++)
      {
          c[i][j]=0;
      }

  }



    ifstream fin("normal_run_data.txt");  //��ȡ

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
  tmp2=0;     //�����һ��id



for(int j=0;j<900000;j++)//ѭ�����
{

    while(a!="ID:")  //�ҵ�id:�������
  {
      fin>>a;
  }
  fin>>a;

      for(k=0;k<50;k++)   //��id�������ҵ���Ӧλ��
      {
         if( a==id[k])
         break;
      }

      if(k==50)       //��id������û�У������
      {
          id[idnum]=a;
          k=idnum;
          idnum++;
      }

      tmp1=tmp2;           //�޸�ת�ƾ���
      tmp2=k;
      c[tmp1][tmp2]=1;


}

for(int i=0;i<idnum;i++)//���id
{
    cout<<id[i]<<endl;
}

  for(int i=0;i<idnum;i++)  //���c
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
