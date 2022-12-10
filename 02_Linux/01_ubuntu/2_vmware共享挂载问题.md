
#手动安装vmware tools

使用cd设备挂载 vmware安装目录下的名为linux的镜像文件

在虚拟机中将压缩包解压到home目录下，然后执行.pl文件。所有安装选默认

#手动挂载
```shell
sudo vmhgfs-fuse .host:/ /mnt/hgfs/ -o allow_other -o uid=1000

```
如果可以正常挂载，将如下命令写入/etc/fstab目录下，重启即可。将会自动挂载

```shell
.host:/  /mnt/hgfs fuse.vmhgfs-fuse  allow_other,defaults   0   0

```

