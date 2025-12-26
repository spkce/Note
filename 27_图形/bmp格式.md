# BMP 
BMP文件格式又称为Bitmap,BMP文件的数据按照从文件头开始的先后顺序分为四个部分

名称 | 大小 
---------|----------
 bmp文件头 | 14Bytes 
 位图信息头 | 40Bytes 
 调色板 | 由颜色索引数决定 
 位图数据 | 由图像尺寸决定 

 我们一般见到的图像以24位图像为主，即R、G、B三种颜色各用8个bit来表示，这样的图像我们称为真彩色，这种情况下是不需要调色板的，也就是位图信息头后面紧跟的就是位图数据了。
 bmp中数据小端存储

 ## BMP文件头
```c++
typedef struct tagBITMAPFILEHEADER { // bmfh 
    WORD    bfType; //占2字节
    DWORD   bfSize; //占4字节
    WORD    bfReserved1; //占2字节
    WORD    bfReserved2; //占2字节
    DWORD   bfOffBits; //占4字节
} BITMAPFILEHEADER;
```
变量 | 地址偏移 | 大小 | 说明
---------|----------|---------|---------
 bfType | 000h | 2Bytes | 必须为0x4d42('BM')
 bfSize | 002h | 4Bytes | 位图文件大小，单位Byte
 bfReserved1 | 006h | 2Bytes | 保留，必须为0
 bfReserved2 | 008h | 2Bytes | 保留，必须为0
 bfOffBits | 00Ah | 4Bytes | 文件头到实际图像数据之间的偏移

### 信息头
```c++
typedef struct tagBITMAPINFOHEADER{ // bmih 
    DWORD  biSize; 
    LONG   biWidth; //4字节
    LONG   biHeight; 
    WORD   biPlanes; 
    WORD   biBitCount 
    DWORD  biCompression; 
    DWORD  biSizeImage; 
    LONG   biXPelsPerMeter; 
    LONG   biYPelsPerMeter; 
    DWORD  biClrUsed; 
    DWORD  biClrImportant; 
} BITMAPINFOHEADER; 
```
变量 | 地址偏移 | 大小 | 说明
---------|----------|---------|---------
 biSize | 000Eh | 4Bytes | BITMAPINFOHEADER字节数
 biWidth | 0012h | 4Bytes | 图像宽度
 biHeight | 0016h | 4Bytes | 图像高度
 biPlanes | 001Ah | 2Bytes | 颜色平面数，总为1
 biBitCount | 001Ch | 2Bytes | 每像素的bit数
 biCompression | 001Eh | 4Bytes | 压缩类型，0不压缩
 biSizeImage | 0022h | 4Bytes | 图像大小，字节为单位
 biXPelsPerMeter | 0026h | 4Bytes | 水平分辨率，像素/米
 biYPelsPerMeter | 002Ah | 4Bytes | 垂直分辨率，像素/米
 biClrUsed | 002Eh | 4Bytes | 使用彩色表中颜色索引数
 biClrImportant | 0032h | 4Bytes | 对图像显示重要影响的索引数目

 ### 调色板
 调色板其实是一张映射表，标识颜色索引号与其代表的颜色的对应关系。它在文件中的布局就像一个二维数组palette[N][4],其中N表示总的颜色索引数，每行的四个元素分别表示该索引对应的B、G、R和Alpha的值，每个分量占一个字节。如不设透明通道时，Alpha为0。因为前面知道，本图有256个颜色索引，因此N = 256。索引号就是所在行的行号，对应的颜色就是所在行的四个元素