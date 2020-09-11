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

### 设置环境变量
在lfs用户的home目录下创建.bash_profile
内容：
```
exec env -i HOME=$HOME TERM=$TERM PS1='\u:\w\$ ' /bin/bash
```
用完全空的环境来取代当前的环境（除了继承 HOME, TERM 和 PS1 变量外）。 这样能保证我们的编译环境不会被宿主系统中不必要的或者有潜在危险的环境变量所影响


非登陆shell（如）是不会读取/etc/profile 或 .bash_profile 文件，而是读取 .bashrc 文件

创建.bashrc内容
```
set +h
umask 022
LFS=/mnt/lfs
LC_ALL=POSIX
LFS_TGT=$(uname -m)-lfs-linux-gnu
PATH=/tools/bin:/bin:/usr/bin
export LFS LC_ALL LFS_TGT PATH
```
**set +h** 命令关闭 bash 的 hash 功能。 Hash 通常是一个有用的特性 —bash 使用一个 hash 表来记录可执行文件的完整路径，以避免为寻找同一个可执行文件而反复搜索 PATH 里的目录。 然而，新工具装好之后就要立即使用，通过关闭 hash 功能， 当要运行程序的时候，shell 将总是搜索PATH 里的目录。 这样新工具一编译好，shell 就可以在 $LFS/tools 目录里找到， 而不是执行所记忆的其它地方的旧版本程序。

**umask 022**将用户文件创建掩码 (umask) 设为 022，使得新创建的文件和目录只有所有者可写， 其他用户只能读取和运行( open(2) 系统调用的默认模式是新文件权限 644，新目录权限 755)。

**LFS** 环境变量应该设为所选择的挂载点

**LC_ALL** 环境变量控制着某些程序的本地化，使其显示的信息遵循特定国家的惯例

**LFS_TGT** 变量设置一个非默认的，但是用于我们建立交叉编译器和链接器以及交叉编译我们的临时工具链时的兼容机器说明。 

> 1. u username 2. 图像界面打开终端 3.执行脚本（进入子shell）为非登陆shell


## 构建临时文件系统
第一步是构建一个新的不依赖于宿主系统的工具链(编译器、汇编器 、 连接器、库文件以及一些有用的软件)。
第二步是利用这个工具链去构建其它基本的工具

 软件都安装在$LFS/tools目录下

## 构建虚拟文件系统

此时开始构建真正的系统

```
mkdir -v $LFS/{dev,proc,sys}
```

### 创建console 和 null 设备
```
mknod -m 600 $LFS/dev/console c 5 1
mknod -m 666 $LFS/dev/null c 1 3
```

### 挂载并填充 /dev 目录

推荐的向 /dev 目录填充设备的方法是在 /dev 上挂载一个虚拟文件系统(比如 tmpfs)， 然后在设备被检测到或被访问到的时候 (通常是在 Udev 引导的过程中) 动态创建设备节点。 因为现在新的系统还没有Udev，并且尚未被引导， 所以就有必要通过手工挂载和填充 /dev 目录。 这可以通过绑定挂载宿主系统的 /dev 目录。 绑定挂载是一种特殊的挂载方式， 允许你创建一个目录或者是挂载点的镜像到其他的地方。 可以使用下面的命令：
```
mount -v --bind /dev $LFS/dev
```
mount --bind命令是将前一个目录挂载到后一个目录上，所有对后一个目录的访问其实都是对前一个目录的访问
>udevd是根据一定的规则在/dev/目录下bai生成du设备zhi文件。比如插入一个u盘， 就会自动生成/dev/sda 这样的文件

### 挂载虚拟内核文件系统

```
mount -vt devpts devpts $LFS/dev/pts
mount -vt tmpfs shm $LFS/dev/shm
mount -vt proc proc $LFS/proc
mount -vt sysfs sysfs $LFS/sys
```