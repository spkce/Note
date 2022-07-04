
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

从PC版本代码的将gen_lex_hash 和lex_hash.h拷贝过来 再touch一下。
```
vmuser@ubuntu:temp$ cp lex_hash.h ../mysql-5.1.73/sql/
vmuser@ubuntu:temp$ cp gen_lex_hash ../mysql-5.1.73/sql/
vmuser@ubuntu:temp$ touch -m ../mysql-5.1.73/sql/gen_lex_hash
vmuser@ubuntu:temp$ touch -m ../mysql-5.1.73/sql/lex_hash.h

```
安装 make install


拷贝的/opt/mysql-5.1.73到开发板的相同目录，注意目录必须相同


## 配置
```
[root@IoT7000A opt]# cat sql.sh
#!/bin/bash
MYSQL_BASE=/opt/mysql-5.1.73
echo "Setup the mysql-5.7.23....."
echo "Create the data path ${MYSQL_BASE}/data..."
mkdir -p ${MYSQL_BASE}/data
echo "Initailize the data directory and create the MySQL grant tables"
${MYSQL_BASE}/bin/mysqld --user=root --basedir=${MYSQL_BASE} --datadir=${MYSQL_BASE}/data --initialize-insecure
##--initialize
echo "copy mysqld config file to /etc/ "
cp my.cnf /etc/my.cnf
echo "Create link to /usr/bin"
ln -s ${MYSQL_BASE}/bin/mysqld /usr/bin/mysqld
ln -s ${MYSQL_BASE}/bin/mysql /usr/bin/mysql
ln -s ${MYSQL_BASE}/bin/my_print_defaults /usr/bin/my_print_defaults
ln -s ${MYSQL_BASE}/bin/mysqladmin /usr/bin/mysqladmin
ln -s ${MYSQL_BASE}/bin/mysqldump /usr/bin/mysqldump
echo "Starting and Stopping MySQL Automatically.... "
cp ${MYSQL_BASE}/support-files/mysql.server /etc/init.d/mysqld
/usr/sbin/update-rc.d mysqld defaults 98
echo "No passwd for mysql, you can use the following command to login: "
echo "mysql -u root --skip-password " echo "[Done]" exit 0;[
```

注意： mysqld在/opt/mysql-5.1.73/libexec 中 把他拷贝到 /opt/mysql-5.1.73/bin下面


gedit /etc/my.conf
```
[mysqld]
datadir=/usr/local/mysql/mysql_data
socket=/tmp/mysql.sock
user=root
#Default to using old password format for compatibility with mysql3.x
#clients (those using the mysqlclient10 compatibility package).
old_passwords=1
[mysqld_safe]
log-error=/var/log/mysqld.log
pid-file=/var/lib/mysql/camelshoko.pid
```

/mysql.server拷贝到/etc/init.d/mysql.server（5.1.73 在share目录下）

安装

usr/local/mysql/bin/mysql_install_db

启动
```
[root@IoT7000A mysql-5.1.73]# /etc/init.d/mysql.server start
Starting MySQL. SUCCESS!
```
至此完成安装


执行时报错：
ERROR 1289 (HY000) at line 24 in file: 'CreateTable.sql': The 'partitioning' feature is disabled; you need MySQL built with '--with-partition' to have it working
源码编译时需要在configure 加--with-partition
报错：
configure: WARNING: unrecognized options: --with-partition
修改为：
--with-plugins=partition

如何生成libqsqlmysql.so
INCLUDEPATH += /opt/qt5.9.5-a7/include \
			/opt/mysql-5.1.73/include
LIBS += -L/opt/mysql-5.1.73/lib/mysql -lmysqlclient


