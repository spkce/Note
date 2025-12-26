# i2c 驱动
i2c设备为lt9611，将mipi-dsi信号转化为hdmi信号。代码仅仅有i2c读写的操作函数，lt9611的业务逻辑没有实现

设备树：
```
am_i2c3: i2c@4093000 {
    compatible = "snps,designware-i2c";
    reg = <0x0 0x4093000 0x0 0x1000>;
    #address-cells = <1>;
    #size-cells = <0>;
    clocks = <&crg CLK_AM_I2C3>;
    clock-names = "i2c_clk";
    clock-select = <50000000>;
    clock-frequency = <100000>;
    resets = <&crg RST_AM_I2C3>;
    reset-names = "i2c_rst";
    interrupts = <GIC_SPI 58 IRQ_TYPE_LEVEL_HIGH>;
    pinctrl-names = "default", "gpio";
    pinctrl-0 = <&a_i2c3_pins>;
    pinctrl-1 = <&a_i2c3_pins_gpio>;
    scl-gpios = <&a_gpio2_port_a 14 GPIO_ACTIVE_HIGH>;
    sda-gpios = <&a_gpio2_port_a 15 GPIO_ACTIVE_HIGH>;
    status = "okay";
    
    lt9611: codec@3b {
            #sound-dai-cells = <0>;
            compatible = "lt9611";
            reg = <0x3b>;
        };
};
```