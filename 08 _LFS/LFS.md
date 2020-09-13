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

### 进入 Chroot 环境
进入 chroot 环境来开始编译安装最终的 LFS 系统
以 root 身份运行以下命令进入构建环境:
```
chroot "$LFS" /tools/bin/env -i \
    HOME=/root TERM="$TERM" PS1='\u:\w\$ ' \
    PATH=/bin:/usr/bin:/sbin:/usr/sbin:/tools/bin \
    /tools/bin/bash --login +h
```

env 命令的参数 -i 的作用是清除所有 chroot 环境变量， 然后，仅仅重设变量 HOME, TERM, PS1 和 PATH 的值。 TERM=$TERM 设定虚拟根环境中的 TERM 的值与 chroot 外面的一样。 这个值是让像 vim 和 less 之类的程序可以正确操作。 如果还需要重新设置其它的值，如 CFLAGS 或 CXXFLAGS ，这里是个不错的位置。

从现在开始，不再需要 LFS 变量了，因为所有的工作都被限制在 LFS 文件系统里。 这是由于已经告诉了 Bash shell， $LFS 是现在的根(/)目录。

 /tools/bin 位于 PATH 的后部， 这就是说 ， 一旦最终版本安装完成， 就不再使用临时工具了。 为了使 shell 无法"记住"可执行二进制代码的位置， 需要通过使用 +h 参数关闭 bash 的散列功能。

注意此时 bash提示符会显示：I have no name! 这是正常的， 因为还没有创建 /etc/passwd 文件。

### 创建系统目录结构
```
mkdir -pv /{bin,boot,etc/opt,home,lib,mnt,opt}
mkdir -pv /{media/{floppy,cdrom},sbin,srv,var}
install -dv -m 0750 /root
install -dv -m 1777 /tmp /var/tmp
mkdir -pv /usr/{,local/}{bin,include,lib,sbin,src}
mkdir -pv /usr/{,local/}share/{doc,info,locale,man}
mkdir -v  /usr/{,local/}share/{misc,terminfo,zoneinfo}
mkdir -pv /usr/{,local/}share/man/man{1..8}
for dir in /usr /usr/local; do
  ln -sv share/{man,doc,info} $dir
done
case $(uname -m) in
 x86_64) ln -sv lib /lib64 && ln -sv lib /usr/lib64 ;;
esac
mkdir -v /var/{lock,log,mail,run,spool}
mkdir -pv /var/{opt,cache,lib/{misc,locate},local}
```

### 创建必需的文件与符号连接
```
ln -sv /tools/bin/{bash,cat,echo,pwd,stty} /bin
ln -sv /tools/bin/perl /usr/bin
ln -sv /tools/lib/libgcc_s.so{,.1} /usr/lib
ln -sv /tools/lib/libstdc++.so{,.6} /usr/lib
ln -sv bash /bin/sh
```
一个常规的Linux系统在 /etc/mtab 中有一个已挂载文件系统的列表,需要为那些用到 /etc/mtab 的程序创建一个空文件
```
touch /etc/mtab
```

为了让 root 用户可以登录而且用户名"root"可以被识别，在这里需要创建相应的 /etc/passwd 和 /etc/group 文件。
```
cat > /etc/passwd << "EOF"
root:x:0:0:root:/root:/bin/bash
bin:x:1:1:bin:/dev/null:/bin/false
nobody:x:99:99:Unprivileged User:/dev/null:/bin/false
EOF
```
x只是占位符
```
cat > /etc/group << "EOF"
root:x:0:
bin:x:1:
sys:x:2:
kmem:x:3:
tty:x:4:
tape:x:5:
daemon:x:6:
floppy:x:7:
disk:x:8:
lp:x:9:
dialout:x:10:
audio:x:11:
video:x:12:
utmp:x:13:
usb:x:14:
cdrom:x:15:
mail:x:34:
nogroup:x:99:
EOF
```

为了消除"I have no name!"提示符，现在启动新的shell
```
exec /tools/bin/bash --login +h

```
程序 login, agetty, 和 init (以及其他一些程序) 使用一些日志文件来记录信息， 比如谁在什么时候登录了系统等等。然而如果这些日志文件不存在， 这些程序则无法写入。 下面初始化这些日志文件，并设置适当的权限：
```
touch /var/run/utmp /var/log/{btmp,lastlog,wtmp}
chgrp -v utmp /var/run/utmp /var/log/lastlog
chmod -v 664 /var/run/utmp /var/log/lastlog
```
/var/run/utmp 文件记录当前登录的用户。/var/log/wtmp 文件记录所有的登录和退出。 /var/log/lastlog 记录每个用户最后的登录信息。/var/log/btmp 文件记录错误的登录尝试。

## 安装 Linux API Headers
为了供系统C库（在LFS中是Glibc）使用，Linux 内核需要公开应用程序接口 (API) 。 这是通过处理Linux内核源程序tar包中的各种C头文件而实现的
确保上一次的活动没有留下失效的文件和依赖：
```
make mrproper
```
现在， 从源码中测试并提取用户可见的内核头文件。因为提取过程会删除目标目录中的原有文件， 所以， 要把它们放在一个中间的本地目录中。 还要从中间目录中删除一些对内核开发者有用，而LFS不需要的隐藏文件。

```
make headers_check
make INSTALL_HDR_PATH=dest headers_install
find dest/include \( -name .install -o -name ..install.cmd \) -delete
cp -rv dest/include/* /usr/include
```

### 安装 Man-pages

### glibc

## 配置动态链接库加载程序
默认情况下，动态链接库加载程序 (/lib/ld-linux.so.2) 搜索 /lib 和 /usr/lib 目录来寻找程序运行时需要使用的动态连接库。 但是， 如果某些库在这两个目录之外， 你就需要把它们的路径加到 /etc/ld.so.conf 文件里，以便动态链接库加载程序能够找到它们。 /usr/local/lib 和 /opt/lib 是两个经常包含动态连接库但又不在默认目录中的目录， 我们要把它们添加到动态链接库加载程序的搜索路径中。
```
cat > /etc/ld.so.conf << "EOF"
# Begin /etc/ld.so.conf

/usr/local/lib
/opt/lib

# End /etc/ld.so.conf
EOF
```



### LFS-Bootscripts

LFS-Bootscripts 软件包包含一套在 LFS 系统开机和关闭时的启动和停止脚本。

包含以下脚本：
checkfs

在挂载之前检查文件系统完整性(日志文件系统和基于网络的文件系统除外)

cleanfs

删除系统重启后就不需要保存了的文件， 例如在 /var/run/和/var/lock/目录下的文件； 重新创建 /var/run/utmp文件并删除可能存在的 /etc/nologin, /fastboot, /forcefsck 文件。

console

为指定的键盘布局读入正确的键盘映射表，并设置屏幕字体。

consolelog

设置内核日志级别来控制信息到达控制台

functions

包含在不同脚本中共用的一些函数，例如错误和状态检查函数。

halt

关闭系统

ifdown

协助 network 脚本停止网络设备

ifup

协助 network 脚本启动网络设备

localnet

设置系统主机名和本地回环(loopback)设备

modules

加载/etc/sysconfig/modules目录列出的内核模块， 使用的参数也会在这个目录给出

mountfs

挂载所有文件系统，有 noauto 标记或者基于网络的文件系统除外。

mountkernfs

加载虚拟内核文件系统，比如proc

network

配置网络接口，如网卡，并设置默认网关（如果可用）

rc

主要的运行级控制脚本，负责让所有其它脚本按符号链接名确定的顺序 一个接一个的运行。

reboot

重启系统

sendsignals

在系统重启或关闭系统之前，确保每一个进程都已经终止了。

setclock

如果硬件时钟没有设置为 UTC 时间，将内核时钟重置为本地时间。

static

提供为网络接口指派静态 IP 地址的功能

swap

启用或禁用交换文件和交换分区

sysctl

如果/etc/sysctl.conf存在，将该文件的系统配置数据加载到运行的内核中

sysklogd

启动或停止系统和内核日志守护进程

template

为其它守护进程创建自定义启动脚本的模板

udev

启动 udev 并在 /dev目录创建设备节点

udev_retry

如果需要，重试失败的udev 消息，并从/dev/.udev拷贝通用的规则文件到/etc/udev/rules.d


### 启动脚本

Linux 使用的是基于运行级(run-levels) 概念的称为 SysVinit 的专用启动工具。

SysVinit(从现在开始我们称之为"init")以运行级的模式来工作，一般有 7 （从 0 到 6）个运行级 (实际上可以有更多的运行级，但都是用于特殊情况而且一般使用不到。 参见 init(8)以获得更多信息)，每个运行级对应于一套设定好的任务， 当启动一个运行级的时候， 计算机就需要执行相应的任务。 默认的运行级是 3，下面是对不同运行级的描述：

0: 关闭计算机
1: 单用户模式
2: 无网络多用户模式
3: 有网络多用户模式
4: 保留作自定义，否则同运行级 3
5: 同运行级 4，一般用于图形界面(GUI)登录(如 X的 xdm 或 KDE的 kdm)
6: 重启动计算机

用来改变运行级别的命令是init <runlevel>，在这里 <runlevel>是目标运行级别。 例如， 要重启计算机， 用户可以执行 init 6 命令，它是 reboot 命令的别名。 同样， init 0 是 halt 命令的别名


在 /etc/rc.d 目录下有很多类似 rc?.d (这里 ? 是运行级别的数值) 和 rcsysinit.d 的目录， 里面都包含许多符号链接，其中一些以 K 字母开头，另外一些以 S 字母开头， 这些链接名在首字母后面都跟着两个数字。 这个 K 的含义是停止 (kill) 一个服务， S 的含义是启动一个服务。而数字则确定这些脚本的启动顺序，从 00 到 99(数字越小执行的越早)。 当 init 转换到其它运行级时， 一些相应的服务会停止， 而另一些服务则会启动。

真正的脚本在 /etc/rc.d/init.d 目录下， 它们完成实际工作， 符号链接都是指向它们的。 停止脚本的链接和启动脚本的链接都指向 /etc/rc.d/init.d 目录下的同一个脚本。 这是因为调用这些脚本时可以使用不同的参数，例如 start, stop, restart, reload, 和 status。 当调用 K 链接时，相应的脚本用 stop参数运行；当调用 S 链接时，相应的脚本用start 参数运行。

上面的说明有一个例外，在 rc0.d 和 rc6.d 目录下以 S 开头的链接不会启动任何东西， 而是用stop参数调用， 来停止某些服务。 这背后的逻辑是， 当用户要重启或关闭系统的时候， 不会要启动什么服务， 只会要系统停止。


以下是脚本参数的描述：

start
启动服务。

stop
停止服务。

restart
关闭服务，然后再启动。

reload
该服务的配置已更新。如果修改了某个服务的配置文件，又不必重启这个服务的时候， 可以使用这个参数

status
显示服务的状态，如果服务正在运行，会显示该服务进程的 PID 。

您可以自由修改启动进程工作的方式(毕竟这是您自己的 LFS 系统)。 我们这里给出的文件只是它们怎样工作的一个示例而已。

### 配置 setclock 脚本

setclock脚本从硬件时钟，也就是 BIOS 或 CMOS 时钟读取时间。如果硬件时钟设置为UTC， 这个脚本会使用 /etc/localtime 文件(这个文件把用户所在的时区告诉 hwclock 程序) 将硬件时钟的时间转换为本地时间。没有办法自动检测硬件时钟是否设置为 UTC 时间， 因此需要手动设置。

当内核检测硬件性能的时候，setclock 由udev运行。 它还可以使用stop参数手动运行， 来将系统时间保存到CMOS时钟。

如果您忘了硬件时钟是不是设置为 UTC 时间了，可以运行 hwclock --localtime --show命令， 这将显示硬件时钟当前的时间。显示的时间符合您的手表的时间，那么硬件时钟设置的是本地时间； 如果 hwclock 显示的不是本地时间，就有可能设置的是 UTC 时间， 可以通过在所显示的 hwclock 时间加上或减去您所在时区的小时数来验证。例如，如果您所在的时区是 MST (美国山区时区)， 已知是 GMT -0700，在本地时间上加 7 小时。

如果你的硬件使用的不是 UTC 时间， 就必须将下面的 UTC 变量值设为 0 (零)

运行下面的命令新建一个 /etc/sysconfig/clock 文件

```
cat > /etc/sysconfig/clock << "EOF"
# Begin /etc/sysconfig/clock

UTC=1

# Set this to any options you might need to give to hwclock, 
# such as machine hardware clock type for Alphas.
CLOCKPARAMS=

# End /etc/sysconfig/clock
EOF
```

###  配置 Linux 控制台

console 和 consolelog 脚本读取 /etc/sysconfig/console文件作为配置信息
决定使用哪种键盘映射和屏幕字体。

### 创建 /etc/inputrc 文件
inputrc 文件为特定的情况处理键盘映射， 这个文件被Bash和大多数其他shell使用的 Readline（输入关系库）用作启动文件。

大多数人并不需要自定义键盘映射，所以下面的命令将创建一个适用于所有登陆用户的全局 /etc/inputrc文件。 如果你需要为某个用户覆盖默认的设置， 你可以在该用户的主目录中创建一个包含自定义键盘映射的 .inputrc 文件。

### Bash Shell 启动文件

shell 程序 /bin/bash (在此之后称为 “the shell”) 使用了一个启动文件集合， 来帮助创造一个运行的环境。每一个文件都有特定的用处， 并且还能使登入与交互环境有所不同。 放在 /etc 目录下的一些文件提供了全局设置。 如果相类似的设置文件出现在某个用户起始文件 夹下， 那么在登入该用户时， 它将替代该全局设置。

使用/bin/login 读取/etc/passwd 文件成功登录后，启动了一个交互登录 shell 。 用命令行可以 启动一个交互非登录 shell (例如 [prompt]$/bin/bash)。 非交互 shell 通常出现在 shell 脚本运行的时候。 之所以称为非交互的， 因为它正在运行一个脚本， 而且命令与命令之间并不等待用户的输入。

当以交互登录方式运行 shell 的时候，会读取 /etc/profile 和 ~/.bash_profile 文件。

下面是一个基本的 /etc/profile文件， 设置了本地语言支持所必需的环境变量， 适当设置这些变量会导致：

将程序的输出转变成本地语言。

正确的将字符分类为字母、数字和其它类，这样做是必要的，可以让bash在非英语 locale 下正确接收命令行输入的非 ASCII 字符。

为国家设置正确的字母排序

适当的默认页面大小

为货币、时间和日期值设置正确的格式


### LFS 系统的设备和模块处理
传统上一般 Linux 系统使用创建静态设备的方法，因此在 /dev目录下创建了大量的设备节点 (有时会有数千个节点)，而不管对应的硬件设备实际上是否存在。这通常是由MAKEDEV脚本完成的， 这个脚本包含许多调用 mknod 程序的命令， 为这个世界上可能存在的每个 设备创建相应的主设备号和次设备号。

**动态创建**
后来发布的 2.6 系列稳定版本内核一种被称为 sysfs的新虚拟文件系统诞生了。sysfs的工作是把系统的硬件配置视图导出给用户空间的进程。 