
## 下载

下载MySQL最新5.1版本的源代码，这里下载的是mysql-5.1.73.tar.gz版本。

http://www.mirrorservice.org/sites/ftp.mysql.com/Downloads/MySQL-5.1/

下载ncurses-5.9.tar.gz：ftp://ftp.gnu.org/gnu/ncurses/ncurses-5.9.tar.gz

## 编译arm使用的ncurses库
ncurses-5.9.tar.gz解压到/home下面

## 配置
在/opt下创建
```
./configure -host=arm-linux-gnueabihf -prefix=/opt/ncurses -enable-static --enable-widec CPPFLAGS="-P" --without-manpages --without-progs --without-tack --without-tests
```
最终 5.8版本没有问题
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


解决：
root@ubuntu:/home/vmuser/ncurses-6.2# ls /usr/bin/strip -al
lrwxrwxrwx 1 root root 22 Aug  5  2019 /usr/bin/strip -> x86_64-linux-gnu-strip