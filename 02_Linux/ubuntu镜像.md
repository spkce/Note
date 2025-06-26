# 下载ubuntu-base 
http://cdimage.ubuntu.com/ubuntu-base/releases/

ubuntu-base-20.04.5-base-arm64.tar.gz

### 解压
mkdir ubuntu_rootfs
sudo tar -xvpf ubuntu-base-20.04.5-base-arm64.tar.gz -C ubuntu_rootfs/ # 需注意解压的参数中 p 参数保留权限:
不能在 windows 目录下解压

# 提取文件
```
#为了可以联网更新软件，拷贝本机的dns配置文件到根文件系统
sudo cp -b /etc/resolv.conf ubuntu_rootfs/etc/ 
-b 在覆盖已存在目标文件前自动创建备份副本，避免数据丢失风险。默认行为：在被覆盖的目标文件‌原名称后添加波浪符~‌生成备份文件

#下载的ubuntu-base是aarch64架构的，因此需要拷贝qemu-aarch64-static到ubuntu/usr/bin/
sudo cp /usr/bin/qemu-aarch64-static ubuntu_rootfs/usr/bin/

# 提取源
sudo cp /etc/apt/sources.list ubuntu_rootfs/etc/apt/sources.list
```


deb http://mirrors.ustc.edu.cn/ubuntu-ports/ focal main multiverse restricted universe
deb http://mirrors.ustc.edu.cn/ubuntu-ports/ focal-backports main multiverse restricted universe
deb http://mirrors.ustc.edu.cn/ubuntu-ports/ focal-proposed main multiverse restricted universe
deb http://mirrors.ustc.edu.cn/ubuntu-ports/ focal-security main multiverse restricted universe
deb http://mirrors.ustc.edu.cn/ubuntu-ports/ focal-updates main multiverse restricted universe
deb-src http://mirrors.ustc.edu.cn/ubuntu-ports/ focal main multiverse restricted universe
deb-src http://mirrors.ustc.edu.cn/ubuntu-ports/ focal-backports main multiverse restricted universe
deb-src http://mirrors.ustc.edu.cn/ubuntu-ports/ focal-proposed main multiverse restricted universe
deb-src http://mirrors.ustc.edu.cn/ubuntu-ports/ focal-security main multiverse restricted universe
deb-src http://mirrors.ustc.edu.cn/ubuntu-ports/ focal-updates main multiverse restricted universe


# 进入根文件系统

sudo chroot ubuntu_rootfs /usr/bin/qemu-aarch64-static /usr/bin/bash -i


# 安装软件
```
apt update
apt upgrade

#安装常用工具包
apt-get install net-tools ethtool ifupdown psmisc nfs-common htop vim rsyslog iputils-ping language-pack-en-base sudo
apt-get install network-manager systemd iputils-ping openssh-sftp-server kmod wireless-tools
apt install build-essential
```

# 安装桌面环境（耗时较长）
apt-get install xubuntu-desktop

# 用户配置
```
# 修改ROOT 密码
passwd root

# 创建 rk3568 账户
useradd -s '/bin/bash' -m -G adm,sudo rk3568
#-s '/bin/bash'‌
#指定用户登录后的默认 Shell 为 Bash，允许用户进行交互式操作（如命令行输入）
#若未指定此选项，系统可能使用默认 Shell（如 /bin/sh）
#‌-m‌
#自动创建用户主目录，默认路径为 /home/rk3568，并复制骨架文件（/etc/skel 中的模板）到该目录
#若省略此选项，需手动创建主目录并设置权限
#‌-G adm,sudo‌
#将用户添加到 ‌附加组‌ adm 和 sudo：
#sudo 组：赋予用户通过 sudo 命令执行特权操作的权限25
#adm 组：允许用户访问系统日志文件（如 /var/log 下的日志
#主组（primary group）未显式指定时，默认创建同名组 rk3568

passwd rk3568

# 修改主机名
echo "rk3568" > /etc/hostname
```

### 清理不必要的包
sudo apt-get autoremove --purge
sudo apt-get clean
sudo apt-get autoremove  

### 删除临时文件
sudo rm -rf /tmp/*
sudo rm -rf /var/cache/apt/archives/*.deb  # 删除旧的.deb包缓存
sudo rm -rf /var/log/*.log                # 删除日志文件（可选）

完成后退出chroot

# 打包镜像

# 清理用户缓存和数据
rm -rf ~/.cache/*  # 删除用户缓存文件
rm -rf ~/.local/share/Trash/*  # 删除Trash中的文件（如果需要）

### 生成空白镜像
dd if=/dev/zero of=linuxroot.img bs=1M count=6000

if=/dev/zero ‌输入源‌为 Linux 系统的 /dev/zero 设备文件，该设备会无限生成空字符（0x00），常用于生成全零文件。
‌of=linuxroot.img‌ ‌输出文件‌名为 linuxroot.img，生成的文件路径为当前目录。若文件已存在，则会被覆盖。
‌bs=1M‌ ‌块大小（Block Size）‌设为 1M，表示每次读取和写入的数据块为 1 MiB（1,048,576 字节）。通过增大块大小可提升操作速度。
‌count=6000‌ ‌操作块数‌为 6000 块，最终文件大小为 6000 × 1 MiB = 6 GiB

### 格式化为ext4 文件系统
mkfs.ext4 linuxroot.img

### 挂在镜像文件, 拷贝数据
```
mkdir rootfs
sudo mount linuxroot.img rootfs/

sudo cp -rfp ubuntu-rootfs/*  rootfs/ # 注意 p 参数
sudo umount rootfs/
```

## 生成镜像的方法二
mkfs.ext4 -d ubuntu-rootfs -r 1 -N 0 -m 5 -L "userfs" -O 64bit linuxroot.img xxxM

xxxM 需要比实际 ubuntu-rootfs/ 大 （sudo du -sh ubuntu_rootfs）


### 检查和修复 rootfs.img 文件系统
e2fsck -p -f linuxroot.img

### 调整大小
resize2fs 命令是调整 ext2、ext3 和 ext4 文件系统大小的有效工具，如果碰到启动不进桌面, 可以尝试不要resize2fs*
resize2fs -M linuxroot.img
