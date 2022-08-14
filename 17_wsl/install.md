# 虚拟机安装
## 在线安装
在windows store安装即可。

## 离线安装
1. 控制面板->程序->启动或关闭Windows功能
勾选“适用于Linux的Windows子系统”与“虚拟机平台

2. 下载离线安装包
https://docs.microsoft.com/en-us/windows/wsl/install-manual#download-using-powershell

下载完后的安装包是.appx结尾的，改为zip后缀后解压。执行ubuntu.exe即可安装

安装后设置密码界面。
可能会有如下错误：

```shell
Enter new UNIX password:
Retype new UNIX password:
Sorry, passwords do not match
passwd: Authentication token manipulation error
passwd: password unchanged
```
解决方法：在linux中执行命令pwconv

然后在powershell中执行（需要管理员用户启动）
 wsl --user root
 然后 passwd root 修改密码

# 路径访问
在windows中 文件浏览器中输入\\wsl$可以访问linux文件

linux 会存放在 C:\Users\{user}\AppData\Local\lxss\{username}
（或C:\Users\{user namae}\AppData\Local\Packages\CanonicalGroupLimited.Ubuntu16.04onWindows_79rhkp1fndgsc\LocalState\rootfs）
/home 文件会存放在 C:\Users\{user}\AppData\Local\lxss\home\

其中 {user} 指的是 windows 的用户，{username} 指的是 linux 里的用户。

window盘符挂载在/mnt下

# 软件安装
sudo vim /etc/apt/source.list
```
deb http://mirrors.aliyun.com/ubuntu/ focal main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ focal main restricted universe multiverse
 
deb http://mirrors.aliyun.com/ubuntu/ focal-security main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ focal-security main restricted universe multiverse
 
deb http://mirrors.aliyun.com/ubuntu/ focal-updates main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ focal-updates main restricted universe multiverse
 
deb http://mirrors.aliyun.com/ubuntu/ focal-proposed main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ focal-proposed main restricted universe multiverse
 
deb http://mirrors.aliyun.com/ubuntu/ focal-backports main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ focal-backports main restricted universe multiverse
```

sudo apt-get update
sudo apt-get upgrade

## 更新软件源
## gcc
sudo apt-get install build-essential