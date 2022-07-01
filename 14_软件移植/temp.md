在Ubuntu下，用户编译源码配置时，sudo ./configure xxx，运行后出现了问题：
sudo: arm-linux-gnueabihf-gcc: command not found

在Terminal中检查，发现arm-linux-gnueabihf-gcc可以在系统的PATH中找到，那为什么还提示说找不到arm-linux-gnueabihf-gcc呢？

原因如下：

1.我添加arm交叉编译器目录到PATH中是放在/etc/profile里的，这是用户的配置文件,我的用户为selwyn。

2.执行./configure时，加了sudo，变成了root的工作环境和root的权限。

根据以上两点，./configure是在root下做的，而arm-linux-gnueabihf-gcc在用户的工作环境中才能找到。所以产生了错误。

那我既要取得root权限，又要具有当前用户的工作环境，怎么处理呢？

方法如下：

执行./configure之前，先用sudo -i命令取得root权限。然后再执行./configure。

su 和 sudo 的区别：

1.共同点：都是root用户的权限；

2.不同点：su仅仅取得root权限，工作环境不变，还是在切换之前用户的工作环境；sudo是完全取得root的权限和root的工作环境。

注意：
1.sudo su切换到root时原用户的环境变量也一并丢失。应使用sudo -i

2.make和make install 也一样
————————————————
版权声明：本文为CSDN博主「selwynzhou」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/weixin_47715886/article/details/108963986


