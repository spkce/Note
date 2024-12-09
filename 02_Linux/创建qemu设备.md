# qemu在线文档
https://wiki.qemu.org/Documentation

# 内核编译 
编译出内核文件（略）
```
./linux-4.14.212/arch/arm/boot/zImage
```

# 使用busybox创建根文件系统
1. 创建rootfs根目录
`mkdir -p rootfs/{dev,etc/init.d,lib}`
2. 把busybox-1.20.2中的文件复制到rootfs根目录下，主要是一些基本的命令
`cp busybox-1.20.2/_install/* -r rootfs/`
3. 把交叉编译工具链中的库文件复制到rootfs根目录的lib文件夹下
`sudo cp -P /usr/arm-linux-gnueabi/lib/* rootfs/lib/`
4. 制作根文件系统镜像 
根文件系统镜像就相当于一个硬盘，就是把上面rootfs根目录中的所有文件复制到这个硬盘中。
 (1) 生成512M大小的磁盘镜像
 `qemu-img create -f raw disk.img 512M`
 (2) 把磁盘镜像格式化成ext4文件系统
 `mkfs -t ext4 ./disk.img`
 (3) 将rootfs根目录中的所有文件复制到磁盘镜像中 操作步骤是：创建挂载点-挂载-复制文件-卸载。
    ```
    mkdir tmpfs 
    sudo mount -o loop ./disk.img tmpfs/  
    sudo cp -r rootfs/* tmpfs/
    sudo umount tmpfs
    ```
# 启动虚拟机
```
qemu-system-arm -M vexpress-a9 -m 512M -kernel ./linux-4.14.212/arch/arm/boot/zImage -dtb  ./linux-4.14.212/arch/arm/boot/dts/vexpress-v2p-ca9.dtb -nographic -append "root=/dev/mmcblk0 rw console=ttyAMA0" -sd disk.img
```