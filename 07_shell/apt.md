# apt 下载
apt download <包名> #文件默认保存到当前目录，无需管理员权限

apt install --download-only <包名> # 所有文件（含依赖包）会缓存到系统目录 /var/cache/apt/archives/


apt install -s #模拟安装


### 清理不必要的包
sudo apt-get autoremove --purge
sudo apt-get clean
sudo apt-get autoremove 

# 清除下载的包
apt clean # /var/cache/apt/archives/目录会被清空


# 查看包安装状态
dpkg -l

# 查看包依赖声明
dpkg-deb -f <包名> depends


