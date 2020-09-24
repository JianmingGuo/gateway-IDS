# -*- coding: utf-8 -*-
"""
Created on Sun Jul 19 20:52:01 2020

@author: unique
"""

import csv

#reader = csv.reader(open('f://DoS Attack_dataset.csv'))
#提取出id，内容，和label
def ycl(read_file,write_file):
    
    with open(write_file, 'w', newline='') as t_file:
        csv_writer = csv.writer(t_file)
        reader = csv.reader(open(read_file))
        id=[]
        for list in reader:
            if len(list)>10:
                #处理id，转化为int类型的标签
                l=[]
                list[1]
                if list[1] in id:
                    b=id.index(list[1])
                else:
                    id.append(list[1])
                    b=id.index(list[1])
                l.append(b)
            
            #处理内容，转化为每一位单独一个输入
            #b=bin(int(list[3],16))[2:]
            #b=b.zfill(8)
            #for i in range(8):
                #l.append(int(b[i]))
                
            #转化为十进制int
                b=int(list[3],16)
                l.append(b)
                b=int(list[4],16)
                l.append(b)
                b=int(list[5],16)
                l.append(b)
                b=int(list[6],16)
                l.append(b)
                b=int(list[7],16)
                l.append(b)
                b=int(list[8],16)
                l.append(b)
                b=int(list[9],16)
                l.append(b)
                b=int(list[10],16)
                l.append(b)
            
           #处理label,转化为0,1标签
                if list[11]=='R':
                    l.append(0)
                else:
                    l.append(1)
                csv_writer.writerow(l)
 


               
ycl('f://Spoofing the drive gear_dataset.csv','f://spoofing1.csv')
ycl('f://Spoofing the RPM gauge_dataset.csv','f://spoofing2.csv')
ycl('f://DoS Attack_dataset.csv','f://dos.csv')
ycl('f://Fuzzy Attack_dataset.csv','f://fuzzy.csv')



