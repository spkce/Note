# 解压一个deb包
dpkg -X package.deb ./extract_folder  # 解压所有安装文件到目标文件夹
dpkg -e package.deb ./extract_folder/DEBIAN #单独获取软件包元数据（control/preinst等脚本）
dpkg -c package.deb 示包内文件层级结构（不实际解压）


# 制作一个deb包
安装依赖工具：apt-get install dpkg-dev build-essential fakeroot

### 准备目录
mkdir -p myapp/DEBIAN
mkdir -p myapp/usr/bin  # 存放可执行文件
mkdir -p myapp/usr/lib  # 存放动态库
mkdir -p myapp/usr/share/doc/myapp  # 存放文档

目录模拟根目录

### 编写控制文件

在DEBIAN目录下创建control文件
```
Package: myapp
Version: 1.0
Architecture: amd64
Maintainer: Your Name <your@email.com>
Description: 简要描述软件功能
Depends: libc6 (>= 2.34)  # 依赖项
```
### 将编译好的二进制文件、配置文件等按系统标准路径放置到对应目录
cp myapp_binary myapp/usr/bin/  # 可执行程序
cp LICENSE myapp/usr/share/doc/myapp/  # 文档

###‌ 维护脚本（可选）‌
preinst：安装前执行的脚本（如停止旧服务）
postinst：安装后执行的脚本（如启用服务）
prerm/postrm：卸载前后的清理脚本

脚本需置于DEBIAN目录，并赋予可执行权限。

### 使用dpkg-deb命令生成包文件：
dpkg-deb --build myapp myapp_1.0_amd64.deb

--build 与 -b 完全等价。

打包时需要以root用户打包，若没有root权限，以fakeroot进行
fakeroot dpkg-deb --build myapp myapp_1.0_amd64.deb

在 fakeroot 中执行 chmod 700 /etc/dummy 不会修改实际文件权限

# 查看包安装状态
dpkg -l

# 查看包依赖声明
dpkg-deb -f <包名> depends

# 安装一个包
dpkg -i myapp_1.0_amd64.deb

# 删除包
sudo apt purge <package-name> # 完全卸载（含配置文件）
sudo apt remove <package-name> # 仅卸载软件（保留配置）