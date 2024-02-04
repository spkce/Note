sudp apt install samba

```shell
//vim /etc/samba/smb.conf
[pixiv]
comment=pixiv
path=/home/pixiv/samba
browseable=yes
readonly=no

```
添加
sudo smbpasswd -a pixiv

删除
sudo smbpasswd -x pixiv

samba restart


# windows 挂载samba
cmd 命令中

```
net use Z: \\192.168.0.109\spkce
```


在ubuntu下创建 samba后发现不能写，用下面方法即可解决：

进入共享目录后：

执行命令：chmod -R go+rwx share/