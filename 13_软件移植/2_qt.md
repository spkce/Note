## 1.下载qt
qt官网：https://www.qt.io/，选择5.9.0版本下载
qt-everywhere-opensource-src-5.9.0.tar.xz

注意：同时有.tar.xz和zip两个包，linux 使用qt-everywhere-opensource-src-5.9.0.tar.xz，不能在window文件系统中解压，一些软连接可能会损坏

## 2. 解压 
xz -d qt-everywhere-opensource-src-5.9.0.tar.xz
tar -xvf qt-everywhere-opensource-src-5.9.0.tar

## 3. 设定编译链
qtbase/mkspecs/linux-arm-gnueabi-g++/ 目录下更改文件qmake.conf

```
# modifications to g++.conf
QMAKE_CC                = /opt/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-gcc
QMAKE_CXX               = /opt/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-g++
QMAKE_LINK              = /opt/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-g++
QMAKE_LINK_SHLIB        = /opt/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-g++

# modifications to linux.conf
QMAKE_AR                = /opt/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-ar cqs
QMAKE_OBJCOPY           = /opt/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-objcopy
QMAKE_NM                = /opt/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-nm -P
QMAKE_STRIP             = /opt/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-strip
```

注意： bash的路径qt读取不到，所以需要用绝对路径

## 4. 产生makefile
```shell
./configure -prefix /opt/qt5.9.5-a7 -opensource -confirm-license -release -strip -shared -xplatform linux-arm-gnueabi-g++ -optimized-qmake -c++std c++11 -make examples -nomake tools -nomake tests -gui -widgets -dbus-runtime --glib=no --iconv=no --pcre=qt -no-openssl --freetype=qt --harfbuzz=qt -no-opengl -linuxfb --xcb=no --libpng=qt --libjpeg=qt --sqlite=qt
```

-prefix /opt/qt5.9.5-a7 是安装目录
-xplatform linux-arm-gnueabi-g++ 是平台，对应qtbase/mkspecs/目录下的文件夹

## 5. 编译
执行 make （可能需要sudo）

## 6. 安装
执行 make install （可能需要sudo）