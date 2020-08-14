
linux下远程登陆其他设备
```
ssh -i root pi@10.10.10.104
```

SCP
```
scp local_file remote_username@remote_ip:remote_folder

scp pi@10.10.10.104:/proc/config.gz ./
```