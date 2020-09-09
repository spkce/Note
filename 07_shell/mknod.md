## mknod
mknod命令用于创建Linux中的字符设备文件（c）和块设备（b）

命令：
```
mknod 设备文件名[/dev/xyz]  b/c  主设备号 次设备号
```


使用 -m可以设置权限
如：

```
mknod -m 600 $LFS/dev/console c 5 1
mknod -m 666 $LFS/dev/null c 1 3
```