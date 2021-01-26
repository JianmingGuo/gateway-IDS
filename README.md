# Gateway-IDS

## 1、 面向车内CAN网络的入侵检测

## siov

基于B/S架构利用Django框架设计实现了入侵检测系统（受服务器性能限制，部分后端算法未集成）

- requirements:
    
    Django-3.0.7
    six-1.15.0

- 在本地8000端口运行(对系统无要求)：
 > python manage.py runserver 127.0.0.1:8000

- 运行截图：

![](pic/iov.jpg)

## gate-web

针对车载互联网关进行信息采集，利用php程序完成网关相关信息的读取并打印到前端界面。

- 运行(以Ubuntu系统为例)

a) 本项目需要利用Apache进行配置，需要安装apache和php  
b) 编辑httpd.conf，使apache支持php  
c) 将项目包放在绝对路径 /var/www/html下  
d) 启动apache服务  
> /etc/init.d/http start  

e) 配置数据库，php代码段中修改代码
>$con = mysqli_connect(ip,database,user_id,password);

e) 访问指定端口实现相关功能，本项目为默认端口
> 127.0.0.1/gateway.html

- 运行截图

![](pic/gateway-web.jpg)

## sourcecode
针对CAN网络的入侵检测模块算法源代码，我们实现了四种入侵检测算法，ANN,LSTM,CART,HTM。本项目包含了数据的预处理和特征分析的代码以及一些数据示例

- HTM

- ANN

- CART

- LSTM

## gateway
In-vehicle gateway intrusion detection based on ARM.C code and arm cross-compilation files



