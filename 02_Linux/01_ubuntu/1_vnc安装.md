
# 目标机
安装vncserver
```shell
sudo apt install tightvncserver
```
安装xfce4
```shell
$ sudo apt install xfce4 xfce4-goodies # 选lightdm

```
设置链接密码：
```shell
vncserver
....
```
修改启动脚本
```shell
#vim ~/.vnc/xstartup
#!/bin/sh
unset SESSION_MANAGER
unset DBUS_SESSION_BUS_ADDRESS
startxfce4 &
[ -x /etc/vnc/xstartup ] && exec /etc/vnc/xstartup
[ -r $HOME/.Xresources ] && xrdb $HOME/.Xresources
xsetroot -solid grey
vncconfig -iconic &
```
启动vncserver
```shell
vncserver -geometry 1920x1080 -depth 24 :1
```
关闭vncserver
```shell
vncserver -kill :1
```

### 若用gnome

```shell
apt-get install gnome-panel gnome-settings-daemon metacity nautilus gnome-terminal
```

修改启动脚本
```shell
#vim ~/.vnc/xstartup
#!/bin/sh
unset SESSION_MANAGER
unset DBUS_SESSION_BUS_ADDRESS
export XKL_XMODMAP_DISABLE=1
export XDG_CURRENT_DESKTOP="GNOME-Flashback:GNOME"
export XDG_MENU_PREFIX="gnome-flashback-"
[ -x /etc/vnc/xstartup ] && exec /etc/vnc/xstartup
[ -r $HOME/.Xresources ] && xrdb $HOME/.Xresources
xsetroot -solid grey
vncconfig -iconic &
#gnome-terminal &    
#nautilus &   
gnome-session --session=gnome-flashback-metacity --disable-acceleration-check &
```