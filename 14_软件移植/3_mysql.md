
## 下载

下载MySQL最新5.1版本的源代码，这里下载的是mysql-5.1.73.tar.gz版本。

http://www.mirrorservice.org/sites/ftp.mysql.com/Downloads/MySQL-5.1/


mysql 依赖ncurses库


## 编译PC的sql
./configure -prefix=/usr/local/mysql

## 编译arm的sql

修改configure
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
./configure --host=arm-linux --enable-static --with-named-curse-libs=/home/vmser/ncurses/lib/libncursesw.a -prefix=/home/vmser/mysql --without-debug --without-docs --without-man --without-bench --with-charset=gb2312 --with-extra-charsets=ascii,latin1,utf8
```





错误

mysql.cc:2687:37: error: invalid conversion from ‘char’ to ‘char*’

field_names[i][num_fields*2]= '\0';
改成
field_names[i][num_fields*2]= "";