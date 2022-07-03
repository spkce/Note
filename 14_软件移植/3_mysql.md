
## 下载

下载MySQL最新5.1版本的源代码，这里下载的是mysql-5.1.73.tar.gz版本。

http://www.mirrorservice.org/sites/ftp.mysql.com/Downloads/MySQL-5.1/


mysql 依赖ncurses库


## 编译PC的sql
./configure -prefix=/opt/mysql-5.1.73

只make 不 make install

## 编译arm的sql

修改configure, 有四处
```
  if test "$cross_compiling" = yes; then
  echo "skip corss_compiling test";
#  { { $as_echo "$as_me:$LINENO: error: in \`$ac_pwd':" >&5
#$as_echo "$as_me: error: in \`$ac_pwd':" >&2;}
#{ { $as_echo "$as_me:$LINENO: error: cannot run test program while cross compiling
#See \`config.log' for more details." >&5
#$as_echo "$as_me: error: cannot run test program while cross compiling
#See \`config.log' for more details." >&2;}
#   { (exit 1); exit 1; }; }; }
else
```

```
./configure --host=arm-linux-gnueabihf --enable-static --with-named-curses-libs=/home/vmuser/ncurses/lib/libncurses.a -prefix=/opt/mysql-5.1.73 --without-debug --without-docs --without-man --without-bench --with-charset=gb2312 --with-extra-charsets=ascii,latin1,utf8
```

编译 make

错误

mysql.cc:2687:37: error: invalid conversion from ‘char’ to ‘char*’

field_names[i][num_fields*2]= '\0';
改成
field_names[i][num_fields*2]= "";

错误：
/bin/bash: ./gen_lex_hash: cannot execute binary file: Exec format error

从PC版本代码的将gen_lex_hash 拷贝过来
```
vmuser@ubuntu:temp$ cp lex_hash.h ../mysql-5.1.73/sql/
vmuser@ubuntu:temp$ cp gen_lex_hash ../mysql-5.1.73/sql/
vmuser@ubuntu:temp$ ls^C
vmuser@ubuntu:temp$ touch -m ../mysql-5.1.73/sql/gen_lex_hash
vmuser@ubuntu:temp$ touch -m ../mysql-5.1.73/sql/lex_hash.h

```
安装 make install


拷贝的/opt/mysql-5.1.73到开发板的相同目录，注意目录必须相同

