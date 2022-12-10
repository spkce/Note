sudp apt install samba

```shell
//vim /etc/samba/smb.conf
[pixiv]
comment=pixiv
path=/home/pixiv/samba
browseable=yes
readonly=no

```
sudo smbpasswd -a pixiv


samba restart
