from django.shortcuts import render,redirect
import os
import shutil

def canids_uplaod_normal(req):
    shutil.rmtree('./canids/train')
    os.mkdir('./canids/train')

    print('uploading normal data...')

    if req.method == "POST":
        file = req.FILES.get("upload",None)
        if not file:
            return render(req,"canids.html",{"errinf":"No files for upload!"})

        f = open('./canids/train/train.csv','wb+')
        for line in file.chunks():
            f.write(line)
        f.close()

    return redirect('/canids/uploadsuccess1/')

def canids_uplaod_test(req):
    shutil.rmtree('./canids/upload')
    os.mkdir('./canids/upload')

    print('upload test data...')

    if req.method == "POST":
        file = req.FILES.get("upload",None)
        if not file:
            return render(req,"canids.html",{"errinf1":"No files for upload!"})

        f = open('./canids/upload/testfile.csv','wb+')
        for line in file.chunks():
            f.write(line)
        f.close()

    return redirect('/canids/uploadsuccess2/')



def canids_uplaodsuccess1(req):
    return render(req,'canids.html',{"errinf":"successful file!"})


def canids_uplaodsuccess2(req):
    return render(req,'canids.html',{"errinf1":"successful file!"})

import csv

def ann_pretreatment(input,output):
    with open(output,'w',newline='') as t_file:
        csv_writer = csv.writer(t_file)
        reader = csv.reader(open(input))
        id=[]
        for list in reader:
            if len(list)>9:
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


import tensorflow.compat.v1    as     tf
tf.disable_eager_execution()  #关闭eager运算
tf.disable_v2_behavior()    #禁用TensorFlow 2.x行为
import numpy as np
#训练步数
training_steps = 30000

def hidden_layer(input_tensor,weight1,bias1,weight2,bias2,weight3,bias3):
    layer1=tf.nn.relu(tf.matmul(input_tensor,weight1)+bias1)
    layer2=tf.nn.relu(tf.matmul(layer1,weight2)+bias2)
    return tf.matmul(layer2,weight3)+bias3

def ann(req):
    input='./canids/train/train.csv'
    output='./canids/train/train_pre.csv'
    print("Processing training data...")
    ann_pretreatment(input,output)
    input1='./canids/upload/testfile.csv'
    ouput1='./canids/upload/testfile_pre.csv'
    print("Processing test data...")
    ann_pretreatment(input1,ouput1)

    print('Analyzing...')

    filename_queue1 = tf.train.string_input_producer([output])

    reader1 = tf.TextLineReader()
    key1, value1 = reader1.read(filename_queue1)
    filename_queue2 = tf.train.string_input_producer([output1])

    reader2 = tf.TextLineReader()
    key2, value2 = reader2.read(filename_queue2)

    record_defaults = [[1.0],[1.0],[1.0],[1.0],[1.0],[1.0],[1.0],[1.0],[1.0],[1.0]]
    col1,col2,col3,col4,col5,col6,col7,col8,col9,col10= tf.decode_csv(value1, record_defaults=record_defaults)
    features = tf.concat([[col1],[col2],[col3],[col4],[col5],[col6],[col7],[col8],[col9]],0)


    init_op = tf.global_variables_initializer()
    local_init_op = tf.local_variables_initializer()
    data=[]
    label=[]

    with tf.Session() as sess:
        coord = tf.train.Coordinator()
        threads = tf.train.start_queue_runners(coord=coord)
        for i in range(10000):
            d,l=sess.run([features,col10])
            data.append(d)
            label.append(l)


        coord.request_stop()
        coord.join(threads)

    col1,col2,col3,col4,col5,col6,col7,col8,col9,col10= tf.decode_csv(value2, record_defaults=record_defaults)
    features = tf.concat([[col1],[col2],[col3],[col4],[col5],[col6],[col7],[col8],[col9]],0)
    with tf.Session() as sess:
        coord = tf.train.Coordinator()
        threads = tf.train.start_queue_runners(coord=coord)
        for i in range(10000):
            d,l=sess.run([features,col10])
            data.append(d)
            label.append(l)


        coord.request_stop()
        coord.join(threads)

        data = np.hstack(data).reshape(-1,9)
        label =np.hstack(label).reshape(-1,1)

    x = tf.placeholder(tf.float32,shape=(None,9),name="x-input")
    y_= tf.placeholder(tf.float32,shape=(None,1),name="y-output")

    weight1 = tf.Variable(tf.truncated_normal([9,50],stddev=0.1))
    bias1 =tf.Variable(tf.constant(0.1,shape=[50]))

    weight2 = tf.Variable(tf.truncated_normal([50,50],stddev=0.1))
    bias2 =tf.Variable(tf.constant(0.1,shape=[50]))

    weight3 = tf.Variable(tf.truncated_normal([50,1],stddev=0.1))
    bias3 =tf.Variable(tf.constant(0.1,shape=[1]))

    sample_size = len(data)
    #输出y
    y = hidden_layer(x,weight1,bias1,weight2,bias2,weight3,bias3)

    #损失函数
    error_loss = tf.reduce_sum(tf.pow(y_-y,2))/sample_size
    tf.add_to_collection("losses",error_loss)

    #加入正则化
    #regularizer = tf.contrib.layers.l2_regularizer(0.01)
    regularizer=tf.keras.regularizers.l2(0.001)
    regularization = regularizer(weight1)+regularizer(weight2)+regularizer(weight3)
    tf.add_to_collection("losses",regularization)

    loss = tf.add_n(tf.get_collection("losses"))

    #定义优化器
    train_op = tf.train.AdamOptimizer(0.05).minimize(loss)
    #train_op = tf.train.GradientDescentOptimizer(0.05).minimize(loss)

    #定义准确率

    with tf.Session() as sess:
        tf.global_variables_initializer().run()
        for i in range(training_steps):
            sess.run(train_op,feed_dict={x:data,y_:label})
            if i%2000 ==0:
                loss_value = sess.run(loss,feed_dict={x:data,y_:label})
                print("After %d steps, losses:%f" %(i,loss_value))