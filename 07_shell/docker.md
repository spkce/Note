# wsl + ubuntu 安装docker
删除所有冲突包
```
 for pkg in docker.io docker-doc docker-compose docker-compose-v2 podman-docker containerd runc; do sudo apt-get remove $pkg; done
```
# 添加Docker的官方GPG密钥:
```
sudo apt-get update
sudo apt-get install ca-certificates curl
sudo install -m 0755 -d /etc/apt/keyrings
sudo curl -fsSL https://download.docker.com/linux/ubuntu/gpg -o /etc/apt/keyrings/docker.asc
sudo chmod a+r /etc/apt/keyrings/docker.asc
```
# 将存储库添加到apt源:
```
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] https://download.docker.com/linux/ubuntu \
  $(. /etc/os-release && echo "${UBUNTU_CODENAME:-$VERSION_CODENAME}") stable" | \
  sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
```
```
sudo apt-get update
```

# 安装docker
```
sudo apt-get install docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin
```
```
docker --version 
```

# 安装 qemu-aarch64-static
```
sudo apt-get install qemu-user-static
```
下载后会有：
```
qemu-aarch64-static       qemu-m68k-static          qemu-nios2-static         qemu-sh4-static
qemu-aarch64_be-static    qemu-microblaze-static    qemu-or1k-static          qemu-sh4eb-static
qemu-alpha-static         qemu-microblazeel-static  qemu-ppc-static           qemu-sparc-static
qemu-arm-static           qemu-mips-static          qemu-ppc64-static         qemu-sparc32plus-static
qemu-armeb-static         qemu-mips64-static        qemu-ppc64abi32-static    qemu-sparc64-static
qemu-cris-static          qemu-mips64el-static      qemu-ppc64le-static       qemu-tilegx-static
qemu-debootstrap          qemu-mipsel-static        qemu-riscv32-static       qemu-x86_64-static
qemu-hppa-static          qemu-mipsn32-static       qemu-riscv64-static       qemu-xtensa-static
qemu-i386-static          qemu-mipsn32el-static     qemu-s390x-static         qemu-xtensaeb-static
```

qemu-aarch64-static 是 QEMU 用户模式工具链中的一个静态编译版本，主要用于在 ‌x86_64 架构‌的宿主机上直接运行 ‌ARM64 (aarch64) 架构‌的二进制程序或容器镜像

允许在 x86_64 机器上无需修改代码直接运行 ARM64 架构编译的应用程序或容器环境。
‌轻量化仿真‌：仅模拟用户空间的指令集，不涉及底层硬件虚拟化（如 CPU、内存控制器等），资源消耗低于 QEMU 系统模式（qemu-system-aarch64）

# 激活binfmt_misc内核模块
sudo docker run --rm --privileged multiarch/qemu-user-static:register

# 启动
```
sudo docker run --rm -it --platform=linux/arm64/v8 -v /usr/bin/qemu-aarch64-static:/usr/bin/qemu-aarch64-static -v /home/spkce/:/mnt arm64v8/ubuntu:20.04 bash
```
--rm‌ ：容器退出后自动删除其文件系统，避免残留临时容器。
‌-it‌启用交互式终端：-i（保持标准输入开放）-t（分配伪终端），允许用户与容器进行交互。
--platform=linux/arm64 将宿主机上的QEMU用户模式模拟器（用于ARM64）挂载到容器内，使x86主机能够解释ARM64指令。
‌arm64v8/ubuntu:20.04 指定官方ARM64架构的Ubuntu 20.04镜像。
‌bash‌ 在容器内启动Bash Shell，进入交互式命令行界面。


### -v 挂载
将宿主机目录挂载到虚拟机里面 -v /home/spkce/:/mnt

### 端口映射 -p
将宿主机8080端口映射到虚拟机80里-p 8080:80

# 常用命令
### 本地镜像
```
docker images
```

