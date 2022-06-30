1,cd /usr/local/src
2,wget https://tukaani.org/xz/xz-5.2.3.tar.gz //下载到/usr/local/src目录下

3,tar xzvf xz-5.2.3.tar.gz

4, cd xz-5.2.3

5, ./configure --prefix=/opt/gnu/xz //将xz软件安装到/opt/gnu/xz目录下

6,make 

7, make install (可能需要sudo)


8,vi /etc/profile
//设置环境变量，在 export PATH USER LOGNAME MAIL HOSTNAME HISTSIZE HISTCONTROL 一行的上面添加如下内容:
export XZ_HOME=/opt/gnu/xz
export PATH=$XZ_HOME/bin:$PATH

9, source /etc/profile //使修改生效