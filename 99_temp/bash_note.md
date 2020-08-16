
# linux下远程登陆其他设备
```
ssh -i root pi@10.10.10.104
```

# SCP
```
scp local_file remote_username@remote_ip:remote_folder

scp pi@10.10.10.104:/proc/config.gz ./
```

# git
只克隆一个commit id， 工程太大clone不到本地可用此方法
```
git clone --depth=1
```
这样的话使用`git branch -a`是看不到其他分支的。
若要clone 其他分支
```
$ git remote set-branches --add origin  origin 'remote_branch_name'
$ git fetch --depth 1 origin remote_branch_name
$ git checkout remote_branch_name
```