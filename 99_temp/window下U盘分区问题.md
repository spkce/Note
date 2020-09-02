
window U盘未分配分区不能创建

1、WIN+R => cmd => diskpart命令进入工具。

2、使用LIST DISK查看所有磁盘，？提示所有命令。

3、SELECT DISK 1将磁盘聚焦到1号磁盘，可以再次用LIST DISK查看是否聚焦，前面会有*号。

4、CLEAN清除磁盘所有信息。