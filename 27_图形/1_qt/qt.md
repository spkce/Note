编译报错：
usr/include/c++/4.9/bits/stl_relops.:67: Parse error at "std"

删除build下的 .qmake.stash解决


# 与linuxfb的接口
src/plugins/platforms/linuxfb/qlinuxfbscreen.cpp


#apt 安装qt
apt install qt5-default

# 查询头文件位置
qmake -query QT_INSTALL_HEADERS  # 输出头文件根目录


# Q_OBJECT
当类声明中包含Q_OBJECT宏时，Qt需要通过moc工具预编译生成元对象代码，未生成会导致vtable缺失
```shell
/usr/bin/ld: /mnt/d/source/video/.//mainwindow.cpp:8: undefined reference to `vtable for MainWindow'
/usr/bin/ld: ./obj/.//main.o: in function `MainWindow::~MainWindow()':
```
可以手动生成扩展
```shell
moc mainwindow.h -o moc_mainwindow.cpp
```