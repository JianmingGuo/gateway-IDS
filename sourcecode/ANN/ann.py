# -*- coding: utf-8 -*-
"""
Created on Mon Jul 20 20:28:35 2020

@author: unique
"""

import tensorflow.compat.v1    as     tf 
tf.disable_eager_execution()  #关闭eager运算
tf.disable_v2_behavior()    #禁用TensorFlow 2.x行为
import numpy as np
#训练步数
training_steps = 30000

#构造数据集
'''data=[]
label=[]
for i in range(200):
    x1=np.random.uniform(-1,1)
    x2=np.random.uniform(0,2)
    
    if x1**2 +x2**2 <= 1:
        data.append([np.random.normal(x1,0.1),np.random.normal(x2,0.1)])
        label.append(0)
    else:
        data.append([np.random.normal(x1,0.1),np.random.normal(x2,0.1)])
        label.append(1)
#翻转
data = np.hstack(data).reshape(-1,2)
label =np.hstack(label).reshape(-1,1)
#reader = csv.reader(open('f://dos1.csv'))'''
#读取csv文件中的内容
filename_queue1 = tf.train.string_input_producer(["f://spoofing1.csv"])

reader1 = tf.TextLineReader()
key1, value1 = reader1.read(filename_queue1)
filename_queue2 = tf.train.string_input_producer(["f://spoofing2.csv"])

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





def hidden_layer(input_tensor,weight1,bias1,weight2,bias2,weight3,bias3):
    layer1=tf.nn.relu(tf.matmul(input_tensor,weight1)+bias1)
    layer2=tf.nn.relu(tf.matmul(layer1,weight2)+bias2)
    return tf.matmul(layer2,weight3)+bias3

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
    #验证        
    #test_accuracy = sess.run(accuracy,feed_dict={x:data1,y_:label1})
    #print(test_accuracy*100)