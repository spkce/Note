
# linux下远程登陆其他设备
```
ssh -p22 pi@10.10.10.104
```

# SCP
```
scp local_file remote_username@remote_ip:remote_folder

scp pi@10.10.10.104:/proc/config.gz ./
```

# 修改terminal显示的路径长短
打开~/.bashrc文件找到下面的代码
```c++
if [ "$color_prompt" = yes ]; then
    PS1='${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ '
else
    PS1='${debian_chroot:+($debian_chroot)}\u@\h:\w\$ '
fi
```
if 分支是终端每行开头的路径，else分支是终端标题路径。w绝对路径，W当前文件目录（短路径）

# xz
xz -d ***.tar.xz



# grep

grep -rn dyFunc

```shell
spkce@pc-spkce:~/source/tmp/dyntest$ grep -rn dyFunc
Binary file a.out matches
dy.cpp:5:void dyFunc()
dy.h:3:extern void dyFunc();
Binary file libdy.so matches
main.cpp:18:    pFun_t pFun = (pFun_t)dlsym(handle, "dyFunc");
```

# nm

```shell

# 查看静态库或动态库定义了哪些函数
nm -n --defined-only xxxx.a
nm -g -C --defined-only xxxx.so
nm -D xxxx.so
# 显示hello.a 中的未定义符号，需要和其他对象文件进行链接.
nm -u hello.o
# 在 ./ 目录下找出哪个库文件定义了close_socket函数. 
nm -A ./* 2>/dev/null | grep "T close_socket"
```

# objdump
```shell
# 查看动态库依赖项
objdump -x xxx.so | grep "NEEDED"

# 查看动态符号表
objdump -T xxx.so
# 查看动态符号表
objdump -t xxx.so
## -T 和 -t 选项在于 -T 只能查看动态符号，如库导出的函数和引用其他库的函数，而 -t 可以查看所有的符号，包括数据段的符号。

objdump -t libmylib.so |c++filt | egrep “foo|bar”

objdump -d -S
```

# readelf
```shell
# 查看静态库定义的函数
readelf -c xxx.a

# 查看静态库定义的函数
readelf -A xxx.so 
```

# grep 
```shell
# 查找文件中内容
grep func . -rn  # func 为要查找的内容

# 正则表达式
grep -E 'xxx' # xxx 为正则表达式
```