# mkimage
内核往往有以下命令编译
make Image
make zImage
make uImage

Image 为普通的内核映像文
zImage为压缩过的内核映像文件
uImage uboot专用的映像文件，它是在zImage之前加上一个长度为64字节的“头”，说明这个内核的版本、加载位置、生成时间、大小等信息

为什么要用uboot 的mkimage工具处理内核映像zImage呢？
因为uboot在用bootm命令引导内核的时候，bootm需要读取一个64字节的文件头，来获取这个内核映象所针对的CPU体系结构、OS、加载到内存中的位置、在内存中入口点的位置以及映象名等等信息。这样bootm才能为OS设置好启动环境，并跳入内核映象的入口点。而mkimage就是添加这个文件头的专用工具
