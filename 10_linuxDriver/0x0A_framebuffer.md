# framebuffer
可以单纯的把framebuffer看作一块内存，这部分内存包含了将要scan out显示的数据
FrameBuffer驱动是一个字符设备，设备节点是/dev/fbX，主设备号为29，次设备号递增，用户可以将Framebuffer看成是显示内存的一个映像，将其映射到进程地址空间之后，就可以直接进行读写操作，而写操作可以立即反应在屏幕上。这种操作是抽象的，统一的。用户不必关心物理显存的位置、换页机制等等具体细节。这些都是由Framebuffer设备驱动来完成的。Framebuffer设备为上层应用程序提供系统调用，也为下一层的特定硬件驱动提供接口；那些底层硬件驱动需要用到这儿的接口来向系统内核注册它们自己。
frambuffer就是linux内核驱动申请的一片内存空间，然后lcd内有一片sram，cpu内部有个lcd控制器，它有个单独的dma用来将frambuffer中的数据拷贝到lcd的sram中去 拷贝到lcd的sram中的数据就会显示在lcd上

## Framebuffer数据结构
### fb_info
fb_info是Linux为帧缓冲设备定义的驱动层接口。它不仅包含了底层函数，而且还有记录设备状态的数据。每个帧缓冲设备都与一个fb_info结构相对应。

```c++
//kernel\include\linux\fb.h
struct fb_info {
    atomic_t count;
    int node;  /*一个FrameBuffer设备的次设备号*/
    int flags;
    struct mutex lock;        /* Lock for open/release/ioctl funcs */
    struct mutex mm_lock;        /* Lock for fb_mmap and smem_* fields */
    struct fb_var_screeninfo var;/* Current var */
    struct fb_fix_screeninfo fix;/* Current fix */
    struct fb_monspecs monspecs;/* Current Monitor specs */
    struct work_struct queue;    /* Framebuffer event queue */
    struct fb_pixmap pixmap;    /* Image hardware mapper */
    struct fb_pixmap sprite;    /* Cursor hardware mapper */
    struct fb_cmap cmap;        /* Current cmap */
    struct list_head modelist;  /* mode list */
    struct fb_videomode *mode;    /* current mode */
#ifdef CONFIG_FB_BACKLIGHT
    struct backlight_device *bl_dev;
    /* Backlight level curve */
    struct mutex bl_curve_mutex;    
    u8 bl_curve[FB_BACKLIGHT_LEVELS];
#endif
#ifdef CONFIG_FB_DEFERRED_IO
    struct delayed_work deferred_work;
    struct fb_deferred_io *fbdefio;
#endif
    struct fb_ops *fbops;
    struct device *device;    /* This is the parent */
    struct device *dev;        /* This is this fb device */
    int class_flag;         /* private sysfs flags */
#ifdef CONFIG_FB_TILEBLITTING
    struct fb_tile_ops *tileops;/* Tile Blitting */
#endif
    char __iomem *screen_base;    /* Virtual address */
    unsigned long screen_size;    /* Amount of ioremapped VRAM or 0 */ 
    void *pseudo_palette;        /* Fake palette of 16 colors */ 
#define FBINFO_STATE_RUNNING    0
#define FBINFO_STATE_SUSPENDED    1
    u32 state;            /* Hardware state i.e suspend */
    void *fbcon_par;    /* fbcon use-only private area */
    void *par;
    struct apertures_struct {
        unsigned int count;
        struct aperture {
            resource_size_t base;
            resource_size_t size;
        } ranges[0];
    } *apertures;
};
```