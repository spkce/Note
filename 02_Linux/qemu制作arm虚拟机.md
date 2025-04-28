# 最简单方法
```c++
//下载qemu
sudo apt-get install qemu-system-arm
//下载UEFI固件 bios镜像，用来引导启动系统
wget http://releases.linaro.org/components/kernel/uefi-linaro/16.02/release/qemu64/QEMU_EFI.fd
//下载ubuntu镜像
//ubuntu-18.04.4-live-server-arm64.iso
// 创建硬盘
qemu-img create ubuntuimg.img 10G
```
安装
```
qemu-system-aarch64 -m 4096 -cpu cortex-a57 -smp 4 -M virt -bios QEMU_EFI.fd -nographic -drive if=none,file=ubuntu-18.04.4-live-server-arm64.iso,id=cdrom,media=cdrom -device virtio-scsi-device -device scsi-cd,drive=cdrom -drive if=none,file=ubuntuimg.img,id=hd0 -device virtio-blk-device,drive=hd0
```

# 其他方法
下载工具
```c++
//下载qemu
sudo apt-get install qemu-system-arm
//安装交叉编译工具链
sudo apt-get install gcc-aarch64-linux-gnu
```
# 下载内核
```
https://mirrors.edge.kernel.org/pub/linux/kernel/
```

# 编译内核

```
sudo apt-get install flex bison libssl-dev libelf-dev ncurses-dev bc # 依赖工具
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- defconfig
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- Image -j8 #生成一个image
```
# busybox
下载busybox
```
https://busybox.net/downloads/

busybox-1.36.1.tar.bz2
```
编译
```
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-
make menuconfig
```
```
Settings --->
[*] Build static binary(shared libs)
[*] Build with debug information
```
```
make -j32
make install # 默认安装路径_install
```

# rootfs image制作

构建ext4 image
```
qemu-img create rootfs.img 10G
mkfs.ext4 rootfs.img
mkdir rootfs //创建rootfs目录
sudo mount rootfs.img rootfs//挂载rootfs.img到rootfs目录
```
将busybox 产生的目录放到镜像中
```
sudo cp -rf _install/* rootfs //拷贝_install到rootfs中
cd rootfs//进入rootfs
sudo mkdir proc sys dev etc etc/init.d //创建关键目录
```
增加rcS内容如下，主要是挂载常使用的虚拟文件系统`sudo vim etc/init.d/rcS`
```
#!/bin/sh
mount -t proc none /proc
mount -t sysfs none /sys
mount -t debugfs none /sys/kernel/debug
```
给rcS增加执行权限
```
sudo chmod +x etc/init.d/rcS
```
取消挂载
```
sudo umount rootfs
```

# qemu启动
```
qemu-system-aarch64 -machine virt -nographic -m size=1024M -cpu cortex-a72 -smp 2 -kernel /home/spkce/source/linux-6.9.12/arch/arm64/boot/Image -drive format=raw,file=/home/spkce/e0/rootfs.img -append "root=/dev/vda rw"
```