下载

```
# 官方镜像安装
sudo wget https://www.python.org/ftp/python/3.6.12/Python-3.6.12.tgz -O /opt/Python-3.6.12.tgz
# 阿里镜像加速
sudo wget http://npm.taobao.org/mirrors/python/3.6.12/Python-3.6.12.tgz -O /opt/Python-3.6.12.tgz
```
解压
```
cd /opt
# 解压
sudo tar xvf Python-3.6.12.tgz
# 
cd Python-3.6.12
```
需要安装的环境
```
# GNU 编辑器集合，GNU 调试器，和其他编译软件所必需的开发库和工具
build-essential 

# OpenSSL的开发库（Secure Sockets Layer toolkit ）
libssl-dev

# zlib是一个压缩解压缩库（compression library）
zlib1g-dev 

# libbz2是高品质块排序文件压缩程序库（high-quality block-sorting file compressor library）bzip2
libbz2-dev 

# GNU Readline 库提供了一组函数供应用程序使用，这些函数允许用户在键入命令行时编辑命令行。（GNU readline and history libraries）
libreadline-dev 

# LLVM 项目是模块化和可重用的编译器和工具链技术的集合。
llvm 

# GNU Ncurses 是一个编程库，它允许用户编写基于文本的用户界面(TUI)。
libncurses5-dev
# 用于终端处理的共享库(宽字符支持)
libncursesw5-dev 

# Tk 是 Tcl“图形工具箱”的扩展，它提供各种标准的 GUI 接口项，以利于迅速进行高级应用程序开发。（Toolkit for Tcl and X11 (default version) ）
tk-dev 

# libffi的作用就相当于编译器，它为多种调用规则提供了一系列高级语言编程接口，然后通过相应接口完成函数调用，底层会根据对应的规则，完成数据准备，生成相应的汇编指令代码。（Foreign Function Interface library）
libffi-dev 

# xz（XZ-format compression library）
liblzma-dev

# xml处理（XSLT 1.0 processing library）
libxslt1-dev

# sqlite3支持库
libsqlite3-dev

# （cross-platform, open-source make system）
cmake

# xml解析 虚包 （XML parsing C library）
libexpat-dev

# （GNU dbm database routines）
libgdbm-dev
```

安装环境
```
sudo apt install build-essential libssl-dev zlib1g-dev libbz2-dev libreadline-dev llvm libncurses5-dev libncursesw5-dev tk-dev libffi-dev liblzma-dev libxslt1-dev libsqlite3-dev cmake libgdbm-dev -y
```

预编译
```
sudo ./configure --prefix=/opt/python3 --enable-optimizations --with-ensurepip=install --enable-loadable-sqlite-extensions
```

编译
```
sudo make -j `grep processor /proc/cpuinfo  | wc -l`
```
grep processor /proc/cpuinfo | wc -l得到的是CPU的虚拟核心数（线程数）

安装
```
sudo make install
```
make install实际上是把生成的执行文件拷贝到之前configure命令指定的目录/opt/python3下

添加软连接
```
sudo ln -s /opt/python3/bin/python3.6 /usr/bin/python
sudo ln -s /opt/python3/bin/pip3 /usr/bin/pip
```