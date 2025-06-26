# 什么是DRM
DRM（ Direct Rendering Manager）即直接渲染管理器。它是为了解决多个程序对 Video Card 资源的协同使用问题而产生的。它向用户空间提供了一组 API，用以访问操纵 GPU。

简单理解，DRM是Linux下的图形渲染架构，用来管理显示输出和分配buffer。应用程序可以直接操纵 DRM的 ioctl 或者是用 framebuffer 提供的接口进行显示相关操作。后来封装成了 libdrm 库，让用户可以更加方便的进行显示控制。

# DRM包含的基本概念
## DRM Framebuffer
它是一块内存区域，可以理解为一块画布，驱动和应用层都能访问它。绘制前需要将它格式化，设定绘制的色彩模式（例如RGB24，YUV 等）和画布的大小（分辨率）

## CRTC
他就是显示输出的上下文，可以理解为扫描仪。CRTC对内连接 Framebuffer 地址，对外连接 Encoder，会扫描 Framebuffer 上的内容，叠加上 Planes 的内容，最后传给Encoder。CRTC 的工作，就是负责把要显示图像，转化为底层硬件层面上的具体时序要求，还负责着帧切换、电源控制、色彩调整等等

## Planes
基本的显示控制单位，每个图像拥有一个Planes，Planes 的属性控制着图像的显示区域、图像翻转、色彩混合方式等，最终图像经过Planes 并通过CRTC 组件，得到多个图像的混合显示或单独显示的等等功能。
平面。它和 Framebuffer 一样是内存地址。它的作用是干什么呢？打个比方，在电脑上，一边打字聊微信一边看电影，这里对立出来两个概念，打字是文字交互，是小范围更新的 Graphics 模式；看电影是全幅高速更新的 Video 模式，这两种模式将显卡的使用拉上了两个极端。
这时Planes就发挥了很好的作用，它给 Video 刷新提供了高速通道，使 Video 单独为一个图层，可以叠加在 Graphic 上或之下，并具有缩放等功能。
Planes 是可以有多个的，相当于图层叠加，因此扫描仪（CRTC）扫描的图像实际上往往是 Framebuffer 和 Planes 的组合（Blending）。


## Encoder
Encoder 的工作则是负责电源管理、视频输出格式封装（比如要将视频输出到HDMI 接口、MIPI接口等）。
它的作用就是将内存的 pixel 像素编码（转换）为显示器所需要的信号。简单理解就是，如果需要将画面显示到不同的设备（Display Device）上，需要将画面转化为不同的电信号，例如 DVID、VGA、YPbPr、CVBS、Mipi、eDP 等。
Encoder 和 CRTC 之间的交互就是我们所说的 ModeSetting，其中包含了前面提到的色彩模式、还有时序（Timing）等。

## Connector
它常常对应于物理连接器 (例如 VGA, DVI, FPD-Link, HDMI, DisplayPort, S-Video等) ，它不是指物理线，在 DRM中，Connector 是一个抽象的数据结构，代表连接的显示设备，从Connector中可以得到当前物理连接的输出设备相关的信息 ，例如，连接状态，EDID数据，DPMS状态、支持的视频模式等。
Connector 连接器负责硬件设备的接入、屏参获取等。


上述的这些组件，最终完成了一个完整的DRM 显示控制过程，如下图所示：
```
[DRM Framebuffer] → [CRTC] → [Encoder] → [Connector] -> hardwave
```

# DRM使用示例

### DRM设备节点
DRM 驱动会在/ dev/dri 下创建3个设备节点:
```
card0
controlD64
renderD128
```
### libdrm库 ：
DRM驱动对用户空间，提供了专门的的调用库libdrm.so，用户空间通过该库可以间接的调用和使用驱动。


## 打开设备
```c++
/* 打开设备有专门的接口：drmOpen ，但此处为方便，使用open函数 */
int fd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        ret = -errno;
        fprintf(stderr, "cannot open '%s': %m\n", node);
        return ret;
    }
```

## 检查DRM的能力
DRM的能力通过drmGetCap接口获取
```c++
/** DRM_IOCTL_GET_CAP ioctl argument type */
struct drm_get_cap {
    __u64 capability;
    __u64 value;
};

int drmGetCap(int fd, uint64_t capability, uint64_t *value)
{
    struct drm_get_cap cap;
    int ret;

    memclear(cap);
    cap.capability = capability;

    ret = drmIoctl(fd, DRM_IOCTL_GET_CAP, &cap);
    if (ret)
        return ret;

    *value = cap.value;
    return 0;
}
```

## 检索Resource
```c++
struct drm_mode_card_res {
    __u64 fb_id_ptr;
    __u64 crtc_id_ptr;
    __u64 connector_id_ptr;
    __u64 encoder_id_ptr;
    __u32 count_fbs;
    __u32 count_crtcs;
    __u32 count_connectors;
    __u32 count_encoders;
    __u32 min_width, max_width;
    __u32 min_height, max_height;
};
typedef struct _drmModeRes {

    int count_fbs;
    uint32_t *fbs;

    int count_crtcs;
    uint32_t *crtcs;

    int count_connectors;
    uint32_t *connectors;

    int count_encoders;
    uint32_t *encoders;

    uint32_t min_width, max_width;
    uint32_t min_height, max_height;
} drmModeRes, *drmModeResPtr;

drmModeResPtr drmModeGetResources(int fd)
```
使用示例
```c++
drmModeResPtr res = drmModeGetResources(fd);
```
## 获取Connector
```c++
typedef struct _drmModeConnector {
    uint32_t connector_id;
    uint32_t encoder_id; /**< Encoder currently connected to */
    uint32_t connector_type;
    uint32_t connector_type_id;
    drmModeConnection connection;
    uint32_t mmWidth, mmHeight; /**< HxW in millimeters */
    drmModeSubPixel subpixel;

    int count_modes;
    drmModeModeInfoPtr modes;

    int count_props;
    uint32_t *props; /**< List of property ids */
    uint64_t *prop_values; /**< List of property values */

    int count_encoders;
    uint32_t *encoders; /**< List of encoder ids */
} drmModeConnector, *drmModeConnectorPtr;

drmModeConnectorPtr drmModeGetConnector(int fd, uint32_t connector);
```
使用示例
```c++
drmModeResPtr res = drmModeGetResources(fd);
drmModeConnector *conn = drmModeGetConnector(fd, res->connectors[i]);
```
## 获取Encoder

```c++
typedef struct _drmModeEncoder {
    uint32_t encoder_id;
    uint32_t encoder_type;
    uint32_t crtc_id;
    uint32_t possible_crtcs;
    uint32_t possible_clones;
} drmModeEncoder, *drmModeEncoderPtr;
drmModeEncoderPtr  drmModeGetEncoder(int fd, uint32_t encoder)；
```
使用示例
```c++
drmModeResPtr res = drmModeGetResources(fd);
drmModeConnector *conn = drmModeGetConnector(fd, res->connectors[i]);
drmModeEncoder *enc = drmModeGetEncoder(fd, conn->encoder_id);
```

## crtc
```c++
struct crtc {
    drmModeCrtc *crtc;
    drmModeObjectProperties *props;
    drmModePropertyRes **props_info;
    drmModeModeInfo *mode;
};
typedef struct _drmModeCrtc {
    uint32_t crtc_id;
    uint32_t buffer_id; /**< FB id to connect to 0 = disconnect */

    uint32_t x, y; /**< Position on the framebuffer */
    uint32_t width, height;
    int mode_valid;
    drmModeModeInfo mode;

    int gamma_size; /**< Number of gamma stops */

} drmModeCrtc, *drmModeCrtcPtr;
```
CRTC 的获取和设置
```c++
//获取
drmModeCrtcPtr drmModeGetCrtc(int fd, uint32_t crtcId)
{
    struct drm_mode_crtc crtc;
    drmModeCrtcPtr r;

    memclear(crtc);
    crtc.crtc_id = crtcId;

    if (drmIoctl(fd, DRM_IOCTL_MODE_GETCRTC, &crtc))
        return 0;

    /*
     * return
     */

    if (!(r = drmMalloc(sizeof(*r))))
        return 0;

    r->crtc_id         = crtc.crtc_id;
    r->x               = crtc.x;
    r->y               = crtc.y;
    r->mode_valid      = crtc.mode_valid;
    if (r->mode_valid) {
        memcpy(&r->mode, &crtc.mode, sizeof(struct drm_mode_modeinfo));
        r->width = crtc.mode.hdisplay;
        r->height = crtc.mode.vdisplay;
    }
    r->buffer_id       = crtc.fb_id;
    r->gamma_size      = crtc.gamma_size;
    return r;
}
//设置
int drmModeSetCrtc(int fd, uint32_t crtcId, uint32_t bufferId,
           uint32_t x, uint32_t y, uint32_t *connectors, int count,
           drmModeModeInfoPtr mode)
{
    struct drm_mode_crtc crtc;

    memclear(crtc);
    crtc.x             = x;
    crtc.y             = y;
    crtc.crtc_id       = crtcId;
    crtc.fb_id         = bufferId;
    crtc.set_connectors_ptr = VOID2U64(connectors);
    crtc.count_connectors = count;
    if (mode) {
      memcpy(&crtc.mode, mode, sizeof(struct drm_mode_modeinfo));
      crtc.mode_valid = 1;
    }

    return DRM_IOCTL(fd, DRM_IOCTL_MODE_SETCRTC, &crtc);
}
```

## FrameBuffer
创建一个dumb buffer
```c++
ret = drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
    if (ret < 0) {
        fprintf(stderr, "cannot create dumb buffer (%d): %m\n",
            errno);
        return -errno;
    }
```

添加一个framBuffer
```c++
    ret = drmModeAddFB(fd, dev->width, dev->height, 24, 32, dev->stride,
               dev->handle, &dev->fb);
    if (ret) {
        fprintf(stderr, "cannot create framebuffer (%d): %m\n",
            errno);
        ret = -errno;
        goto err_destroy;
    }
```
准备map
```c++
    memset(&mreq, 0, sizeof(mreq));
    mreq.handle = dev->handle;
    ret = drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
    if (ret) {
        fprintf(stderr, "cannot map dumb buffer (%d): %m\n",
            errno);
        ret = -errno;
        goto err_fb;
    }
```
进行map
```c++
/* perform actual memory mapping */
    dev->map = mmap(0, dev->size, PROT_READ | PROT_WRITE, MAP_SHARED,
                fd, mreq.offset);
    if (dev->map == MAP_FAILED) {
        fprintf(stderr, "cannot mmap dumb buffer (%d): %m\n",
            errno);
        ret = -errno;
        goto err_fb;
    }
```