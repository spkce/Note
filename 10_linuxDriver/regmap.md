# Regmap
regmap是Linux内核为了减少慢速I/O在驱动上的冗余开销，提供了一种通用的接口来操作硬件寄存器.
regmap的应用场景有如下所示：
1. 硬件寄存器操作，比如选用通过I2C/SPI接口来读写设备的内部寄存器，或者需要读写SOC内部的硬件寄存器。
2. 提高代码复用性和驱动一致性，简化驱动开发过程。
3. 减少底层 I/O 操作次数，提高访问效率。

## regmap框架分为三层：

1. 底层物理总线：regmap就是对不同的物理总线进行封装，目前regmap支持的物理总线有i2c、i3c、spi、mmio、sccb、sdw、slimbus、irq、spmi和w1。
2. regmap核心层，用于实现regmap，不用关心具体实现。
3. regmapAPI抽象层，regmap向驱动编写人员提供的API接口，驱动编写人员使用这些API接口来操作具体的芯片设备，也是驱动编写人员重点要掌握的。

# regmap api

## 初始化和退出
regmap支持多种物理总线，比如I2C和SPI，需要根据所使用的接口来选择合适的regmap初始化函数.一般会在probe函数中初始化regmap_config，然后申请并初始化regmap
```c++
struct regmap * regmap_init_spi(struct spi_device *spi, const struct regmap_config *config)
struct regmap * regmap_init_i2c(struct i2c_client *i2c, const struct regmap_config *config)
//5.14版本用devm_regmap_init
```
退出驱动的时候需要释放掉申请到的regmap，不管是什么接口，全部使用regmap_exit这个函数来释放regmap
```
void regmap_exit(struct regmap *map)
```
## regmap_config
```c++
struct regmap_config {
    /* 基础标识与总线类型 */
    const char *name;               // Regmap 实例名称（调试用）
    enum regmap_bus_type bus_type;  // 总线类型（如 REGMAP_BUS_I2C、REGMAP_BUS_SPI、REGMAP_BUS_MMIO 等）
 
    /* 寄存器与值的位宽配置 */
    int reg_bits;                   // 寄存器地址的位宽（如 8/16/32 位）
    int val_bits;                   // 寄存器值的位宽（如 8/16/32 位）
    int pad_bits;                   // 地址/值与实际总线传输间的填充位（通常为 0）
 
    /* 字节序与对齐方式 */
    enum regmap_endian reg_format_endian;  // 寄存器地址的字节序（大端/小端/不转换）
    enum regmap_endian val_format_endian;  // 寄存器值的字节序（同上）
    bool reg_stride;                // 地址自动递增步长（默认 1，若硬件地址不连续需手动设置）
 
    /* 访问约束与超时 */
    unsigned int max_register;      // 设备支持的最大寄存器地址（0 表示无限制）
    unsigned int fast_io;           // 是否启用快速 IO（1 启用，依赖硬件特性）
    unsigned long read_flag_mask;   // 读操作的标志掩码（如 I2C 的 RD 位）
    unsigned long write_flag_mask;  // 写操作的标志掩码（如 I2C 的 WR 位）
 
    /* 缓存策略 */
    enum regmap_cache_type cache_type;  // 缓存类型（无缓存/读缓存/写缓存/读写缓存）
    unsigned int cache_size;            // 缓存大小（字节数，0 表示自动计算）
    bool use_single_read;               // 强制单次读（即使支持批量读）
    bool use_single_write;              // 强制单次写（即使支持批量写）
 
    /* 中断与事件处理 */
    int irq;                          // 设备关联的中断号（若需要）
    unsigned long irq_flags;          // 中断触发标志（如 IRQF_SHARED）
 
    /* 自定义回调函数 */
    int (*reg_read)(struct regmap *map, unsigned int reg, unsigned int *val);  // 自定义读回调
    int (*reg_write)(struct regmap *map, unsigned int reg, unsigned int val); // 自定义写回调
    void (*lock)(struct regmap *map);                                         // 自定义锁函数
    void (*unlock)(struct regmap *map);                                       // 自定义解锁函数
 
    /* 调试与日志 */
    bool verbose;                     // 是否启用详细日志（调试用）
    struct dentry *debugfs;           // DebugFS 挂载点（用于调试）
};
```
​**bus_type**​：指定底层总线类型，决定 Regmap 如何与总线驱动交互。常见值包括
REGMAP_BUS_I2C：I2C 总线（需配合 struct i2c_client 使用）
REGMAP_BUS_SPI：SPI 总线（需配合 struct spi_device 使用）
REGMAP_BUS_MMIO：内存映射 IO（MMIO，直接访问物理内存地址）
REGMAP_BUS_HOST：主机侧总线（如用于连接外部芯片的专用总线）

​**reg_bits**​：寄存器地址的二进制位数（如 8 位地址对应 reg_bits=8）。
示例：若设备寄存器地址范围是 0x00 ~ 0xFF，则 reg_bits=8。
​**val_bits**​：寄存器值的二进制位数（如 16 位值对应 val_bits=16）。
示例：若寄存器是 8 位宽（每次读写 1 字节），则 val_bits=8；若是 16 位宽（大端存储），则 val_bits=16。
​**pad_bits**​：地址或值与实际总线传输字节的填充位数（通常为 0）。
示例：某些 SPI 设备需要在地址后填充 4 位校验位，则 pad_bits=4。
​**cache_type**​：缓存类型，可选值：
REGMAP_CACHE_NONE：无缓存（每次读写直接访问硬件）。
REGMAP_CACHE_R：只读缓存（仅缓存读操作结果，写操作后失效）。
REGMAP_CACHE_W：写缓存（写操作先更新缓存，读操作从缓存读取，需配合 regmap_write 自动同步）。
REGMAP_CACHE_RW：读写缓存（读写均通过缓存，需手动调用 regmap_sync() 同步到硬件）。

## 读写
```c++
int regmap_read(struct regmap *map, unsigned int reg, unsigned int *val)
int regmap_write(struct regmap *map, unsigned int reg, unsigned int val)
int regmap_update_bits (struct regmap *map, unsigned int reg, unsigned int mask, unsigned int val) //此函数用来修改寄存器指定的bit，bit位由mask知道
```
连续写寄存器
该函数将 val 缓冲区中的 val_count 个数据项，依次写入从 reg 开始的连续寄存器中
```c++
int regmap_bulk_write(struct regmap *map, unsigned int reg, const void *val, size_t val_count);
```

# devm_regmap_init
devm_ 前缀的函数（如 devm_regmap_init）会自动管理内存，当设备被移除时，相关的资源（如注册表）会自动释放，无需手动调用释放函数
devm_regmap_init‌是一个通用的初始化函数，用于为支持 regmap 框架的设备创建和初始化一个 struct regmap 实例。它需要一个 struct regmap_bus 类
devm_regmap_init_i2c是一个专用的初始化函数，专门用于 I2C 总线设备。它内部已经集成了 I2C 的总线操作细节（如调用 i2c_master_send 等函数），用户无需手动提供总线接口，直接传入 i2c_client 和配置即可
```c++
struct regmap *devm_regmap_init_i2c(struct device *dev,
                                    const struct regmap_bus *bus,
                                    const void *bus_context,
                                    const struct regmap_config *config);

struct regmap *devm_regmap_init(struct device *dev,
                                const struct regmap_bus *bus,
                                const void *bus_context,
                                const struct regmap_config *config);
```
