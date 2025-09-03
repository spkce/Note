# wsl 没有网络
关闭WSL：使用命令wsl --shutdown
使用Netsh工具重置网络配置：
netsh winsock reset：重置Winsock目录。
netsh int ip reset all：重置所有IP设置。
netsh winhttp reset proxy：重置Windows HTTP代理设置。
ipconfig /flushdns：刷新DNS缓存‌