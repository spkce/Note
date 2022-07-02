首先查看系统中磁盘信息，命令为：fdisk -l;

格式化命令为：mkfs.ext4 /dev/vdb

在根目录下创建一个data目录
mkdir /data

mount /dev/vdb /data/


5、修改fstab，使得系统启动时自动挂载磁盘，
编辑fstab默认启动文件命令：

使用命令查看： vi /etc/fstab
 
在其中添加一行： 
/dev/vdb /data ext4 defaults 0 0
6、输入：sync 将缓存写入服务器；
7、执行命令：init -6 重启服务器；
8、重启服务器后，输入命令： df -lh 查看该磁盘是否正常挂载。或者使用 lsblk
