# LFS

## 创建分区

使用cfdisk及fdisk创建分区 
一般创建2个: 一个系统分区，一交换分区

此处命名创建的系统分区为hda5，交换分区为hda6

### 格式化分区

将hda5格式化为ext3
```
mke2fs -jv /dev/hda5
```

将hda6初始化
```
mkswap /dev/hda6
```
### 挂载分区
设置一个lfs文件路径的环境变量
```
export LFS=/mnt/lfs
```
将hda5分区挂载到$LFS路径
```
mkdir -pv $LFS
mount -v -t ext3 /dev/hda5 $LFS
```
设置交换分区
```
swapon -v /dev/hda6
```

### 创建代码目录
```
mkdir -v $LFS/sources

chmod -v a+wt $LFS/sources
//目录设置成可写和sticky（粘住位）。“Sticky”的意思是，即使有多个用户对这个目录有可写权限， 只有文件所有者可以删除设置 sticky 的目录中的文件
```

### 创建 $LFS/tools 目录

tool目录为临时系统工具存放的地方。方便删除

以root运行
```
mkdir -v $LFS/tools
```
在宿主系统上创建tool的符号链接
```
ln -sv $LFS/tools /tools
```

### 创建一个用户

创建组
```
groupadd lfs
```

创建用户
```
useradd -s /bin/bash -g lfs -m -k /dev/null lfs
```
>-s /bin/bash
指定 bash 为 lfs 用户的默认 shell。
-g lfs
该选项将 lfs 用户添加到 lfs 组。
-m
该选项为 lfs 用户创建 home 目录。
-k /dev/null
这个参数通过将输入位置修改为特殊的空设备来防止从框架目录(默认是 /etc/skel) 拷贝文件。
lfs
这是所创建的组和用户的实际名字

设置密码
```
passwd lfs
```
通过把 lfs 用户设置为 $LFS/tools 目录的所有者来授予 lfs 用户对该目录的完全访问权限
```
chown -v lfs $LFS/tools
chown -v lfs $LFS/sources
```

切换到lfs用户登陆
```
su - lfs
```