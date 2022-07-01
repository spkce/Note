
## 编译arm使用的ncurses库
ncurses-5.9.tar.gz解压到/home下面

## 配置
在/opt下创建
```
./configure -host=arm-linux-gnueabihf -prefix=/opt/ncurses -enable-static --enable-widec CPPFLAGS="-P" --without-manpages --without-progs --without-tack --without-tests
```

```
./configure --host=arm-linux-gnueabihf --prefix=/home/vmuser/ncurses --enable-widec --with-shared
```

错误
```
In file included from ../ncurses/curses.priv.h:283:0,
                 from ../ncurses/lib_gen.c:19:
_46863.c:835:15: error: expected ‘)’ before ‘int’
../include/curses.h:1594:56: note: in definition of macro ‘mouse_trafo’
 #define mouse_trafo(y,x,to_screen) wmouse_trafo(stdscr,y,x,to_screen)
                                                        ^
Makefile:790: recipe for target '../objects/lib_gen.o' failed
make[1]: *** [../objects/lib_gen.o] Error 1
make[1]: Leaving directory '/home/ran/workspace/ncurses-5.9/ncurses'
Makefile:109: recipe for target 'all' failed
make: *** [all] Error 2
```

去掉include/curses.tail文件的 104行注释/* generated */
```
sudo vim include/curses.tail
103 extern NCURSES_EXPORT(bool)    wmouse_trafo (const WINDOW*, int*, int*, bool);
104 extern NCURSES_EXPORT(bool)    mouse_trafo (int*, int*, bool);              /* generated */
```
错误：
```
In file included from ../c++/cursesm.h:39:0,
                 from ../c++/cursesm.cc:35:
../c++/cursesp.h: In member function ‘T* NCursesUserPanel<T>::UserData() const’:
../c++/cursesp.h:256:43: error: no matching function for call to 
‘NCursesUserPanel<T>::get_user() const’
     return reinterpret_cast<T*>(get_user ());
```
编译器版本过高。解决方法有两个，一是更新ncurses到6.2版本，或者降低编译器版本。


解决： 强制改链接
root@ubuntu:/home/vmuser/ncurses-6.2# ls /usr/bin/strip -al
lrwxrwxrwx 1 root root 22 Aug  5  2019 /usr/bin/strip -> x86_64-linux-gnu-strip
ln -s /opt/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-strip /usr/bin/strip

还有其他问题

* 最终5.8版本没有问题

编译ncurse库时，make install出现如下错误：
./run_tic.sh: line 113: /usr/local/bin/tic: cannot execute binary file
? tic could not build /usr/local/share/terminfo
make[1]: *** [install.data] 错误 1
make[1]: Leaving directory `/opt/ncurses/misc'
make: *** [install] 错误 2
该错误提示：/usr/local/bin/tic不是可执行的二进制文件
file /usr/local/bin/tic发现它是arm平台的，当然无法在pc上运行
[root@localhost ncurses]# file /usr/local/bin/tic
/usr/local/bin/tic: ELF 32-bit LSB executable, ARM, version 1 (ARM), for GNU/Linux 2.4.3, statically linked, for GNU/Linux 2.4.3, not stripped
如果pc机上已经安装过ncurses（ncurses是配置内核、根文件系统等需要用到make menuconfig必备的库），那么pc上一定有可以在pc运行的tic。此时，我们find一下tic
[root@localhost ncurses]# find / -name tic
/opt/ncurses/progs/tic
/usr/bin/tic
/usr/local/bin/tic
机子上总共有3个tic，/opt/ncurses/progs/tic是我们刚刚编译的arm版tic，而/usr/local是我们ncurse库的安装点，因此，/usr/local/bin/tic也是arm版tic。最后只剩下/usr/bin/tic就是x86版的tic。我们可以file一下这3个文件，就知道到底是不是这样：
[root@localhost ncurses]# file /usr/bin/tic
/usr/bin/tic: ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), for GNU/Linux 2.6.9, statically linked, for GNU/Linux 2.6.9, not stripped
[root@localhost ncurses]# file /usr/local/bin/tic
/usr/local/bin/tic: ELF 32-bit LSB executable, ARM, version 1 (ARM), for GNU/Linux 2.4.3, statically linked, for GNU/Linux 2.4.3, not stripped
[root@localhost ncurses]# file /opt/ncurses/progs/tic
/opt/ncurses/progs/tic: ELF 32-bit LSB executable, ARM, version 1 (ARM), for GNU/Linux 2.4.3, statically linked, for GNU/Linux 2.4.3, not stripped
接下来，我们看看环境变量PATH
[root@localhost ncurses]# echo $PATH
/opt/crosstool/gcc-4.1.1-glibc-2.3.2/arm-linux/bin:/usr/lib/qt-3.3/bin:/usr/kerberos/sbin:/usr/kerberos/bin:/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/usr/X11R6/bin:/root/dev_home/tools/bin:/root/bin:/root/dev_home/tools/bin:/opt/host/armv4l/bin:/root/dev_home/tools/bin
从PATH内容可以看出，/usr/local/bin在PATH中的位置比/usr/bin前。因此，在搜索tic时，最先找到的是/usr/local/bin/tic，而不是/usr/bin/tic，这才引起了先前的错误。解决方法很简单：只要让先找到的tic是/usr/bin/tic就可以避免该错误！