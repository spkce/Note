
# linux下远程登陆其他设备
```
ssh -p22 pi@10.10.10.104
```

# SCP
```
scp local_file remote_username@remote_ip:remote_folder

scp pi@10.10.10.104:/proc/config.gz ./
```

# 修改terminal显示的路径长短
打开~/.bashrc文件找到下面的代码
```c++
if [ "$color_prompt" = yes ]; then
    PS1='${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ '
else
    PS1='${debian_chroot:+($debian_chroot)}\u@\h:\w\$ '
fi
```
if 分支是终端每行开头的路径，else分支是终端标题路径。w绝对路径，W当前文件目录（短路径）
