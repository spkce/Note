# YUV420核心原理
YUV 分别由一个称为 Y（相当于灰度）的“亮度”分量（Luminance or Luma）和两个称为 U（蓝色投影 Cb）和 V（红色投影 Cr）的“色度”分量（Chrominance or Chroma）表示，由此得名.
仅有 Y 分量而没有 UV 分量信息，一样可以显示完整的黑白（灰度）图像，解决了模拟信号电视黑白与彩色的兼容问题

Y（亮度）：全分辨率存储（每个像素独立）
U/V（色度）：每2x2像素共享一组UV值，水平和垂直分辨率减半

```c++
// 计算YUV420图像字节数
int y_size = width * height;          // Y分量
int uv_size = (width/2) * (height/2); // U或V分量
int total_size = y_size + uv_size * 2; // Y + U + V
```

```c++
// RGB转YUV分量（范围：Y∈[0,255], UV∈[0,255]）
void rgb_to_yuv(uint8_t r, uint8_t g, uint8_t b, uint8_t& y, uint8_t& u, uint8_t& v) {
    y = 0.299*r + 0.587*g + 0.114*b;
    u = -0.1687*r - 0.3313*g + 0.5*b + 128;  // 加128避免负值
    v = 0.5*r - 0.4187*g - 0.0813*b + 128;
}
```
## 存储格式
### 平面格式 （YUV420P）

```
[YYYY...][UUU...][VVV...]

Y0 Y1 Y2 Y3 Y4 ... Y15   // 16字节
U0 U1 U2 U3             // 4字节（2x2块均值）
V0 V1 V2 V3             // 4字节
```
### 半平面格式（YUV420SP/NV12）
```
[YYYY...][U0 V0 U1 V1 ...]
```