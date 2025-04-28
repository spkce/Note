


# autoconf 编译安装


```
wget https://ftp.gnu.org/gnu/autoconf/autoconf-2.69.tar.gz
tar -xzvf autoconf-2.69.tar.gz
cd autoconf-2.69
./configure --prefix=/mnt/ext4_disk/tools/autoconf-2.69
make && sudo make install
```
添加环境变量：
```
export PATH=/mnt/ext4_disk/tools/autoconf-2.69/bin:$PATH
```
# automake 编译安装
```
wget https://ftp.gnu.org/gnu/automake/automake-1.15.tar.gz
tar -xzvf automake-1.15.tar.gz
cd automake-1.15
./configure --prefix=/mnt/ext4_disk/tools/automake-1.15
make && sudo make install
```
添加环境变量：
```
export PATH=/mnt/ext4_disk/tools/automake-1.15/bin:$PATH
```



# gcc编译准备

用gcc-13编译9.2.0似乎不行

准备gcc-9.2.0
cd gcc-9.2.0

先执行./contrib/download_prerequisites下载需要的依赖包

一般情况下可能会有gmp包下载不下来，手动下载
wget https://ftp.gnu.org/gnu/gmp/gmp-6.1.0.tar.bz2
解压
```
tar -xf gmp-6.1.0.tar.bz2
tar -xf mpfr-3.1.4.tar.bz2
tar -xf mpc-1.0.3.tar.gz
tar -xf isl-0.18.tar.gz
```
修改名字
```
 mv mpc-1.0.3 mpc
 mv mpfr-3.1.4 mpfr
 mv gmp-6.1.0 gmp
 mv isl-0.18 isl
```

# gcc编译准备
创建安装的目录aarch64-gcc-9.2.0

开始编译
```
mkdir build 
cd build 
make distclean
../configure --target=aarch64-linux-gnu --prefix=/mnt/ext4_disk/tools/aarch64-gcc-9.2.0 --with-sysroot=/mnt/ext4_disk/qemu-arm-linux/rootfs --disable-shared --enable-static  --disable-multilib --enable-languages=c,c++ 
```
--target=aarch64-linux-gnu指定了目标架构为ARM64
--prefix指定了安装目录。

sysroot下面可能需要确保/usr/include （/mnt/ext4_disk/qemu-arm-linux/rootfs/usr/include）目录存在。若不存在可以从aarch64-linux-gnu工具链中（/usr/aarch64-linux-gnu/include/）提取出include

# 安装
```
make install
```
最后在/mnt/ext4_disk/tools/aarch64-gcc-9.2.0目录下
```
spkce@ubuntu20.04:~/e0/tools/aarch64-gcc-9.2.0$ ls
aarch64-linux-gnu  bin  include  lib  libexec  share
spkce@ubuntu20.04:~/e0/tools/aarch64-gcc-9.2.0$ ls bin/
aarch64-linux-gnu-c++  aarch64-linux-gnu-gcc-9.2.0   aarch64-linux-gnu-gcov
aarch64-linux-gnu-cpp  aarch64-linux-gnu-gcc-ar      aarch64-linux-gnu-gcov-dump
aarch64-linux-gnu-g++  aarch64-linux-gnu-gcc-nm      aarch64-linux-gnu-gcov-tool
aarch64-linux-gnu-gcc  aarch64-linux-gnu-gcc-ranlib
spkce@ubuntu20.04:~/e0/tools/aarch64-gcc-9.2.0$ ls lib/
gcc  libcc1.a  libcc1.la  libcc1.so  libcc1.so.0  libcc1.so.0.0.0
```