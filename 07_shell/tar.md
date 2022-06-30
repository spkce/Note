```
压缩文件 或者文件夹 并在当前目录下生成压缩包
tar -zcvf rumenz.tar.gz rumenz.txt
解压到当前目录
tar -xzvf rumenz.tar.gz
解压到 /tmp目录
tar -xzvf rumenz.tar.gz -C /tmp

打包一个文件,不压缩

tar -cvf rumenz.tar rumenz.txt

打包并压缩一个文件(使用gzip压缩)

 tar -zcvf rumenz.tar.gz rumenz.txt

打包并压缩一个文件(使用bzip2压缩)

tar -jcvf rumenz.tar.bz2 rumenz.txt
```