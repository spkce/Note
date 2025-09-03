
i2cset [-y] <bus> <addr> <val> [mode]
<bus>：I2C总线编号（如1或2）‌12
<addr>：目标设备的7位十六进制地址（如0x1A）
<val>：要写入的数据值（十六进制或十进制）‌
[mode]：可选传输模式（b字节、w字、iI2C块等）‌
-y：禁用交互确认提示（自动化脚本常用）


读取设备地址0x35的寄存器0x10
i2ctransfer -y 0 w1@0x51 0x10 r1  


i2cdetect -y <总线编号>  # 扫描总线所有设备

i2cdump -f -y <总线号> <设备地址>