
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/device.h>
#include <drm/drm.h>
#include <drm/drm_connector.h>
#include <drm/drm_crtc.h>

static int i2c_read(void *context, unsigned int reg, unsigned int *val)
{
	struct i2c_client *i2c = context;
    struct i2c_msg xfer[2] = {0};
    uint8_t regAddr = (uint8_t)reg;
    uint8_t buf = 0;
    int ret;

	xfer[0].addr = i2c->addr;
	xfer[0].flags = 0;
	xfer[0].len = 1;
	xfer[0].buf = (uint8_t*)&regAddr;

	/* Read data */
	xfer[1].addr = i2c->addr;
	xfer[1].flags = I2C_M_RD;
	xfer[1].len = 1;
	xfer[1].buf = (uint8_t*)&buf;

	ret = i2c_transfer(i2c->adapter, xfer, 2);
	if (ret < 0)
		return ret;
	else if (ret != 2)
		return -EIO;
    
    *val = buf;

    return 0;
}

static int i2c_write(void *context, unsigned int reg, unsigned int val)
{
    struct i2c_client *i2c = context;
	struct i2c_msg xfer[1];
    int ret;
    uint8_t buf[2] = {0};
    buf[0] = (uint8_t)reg;
    buf[1] = (uint8_t)val;

    xfer[0].addr = i2c->addr;
	xfer[0].flags = 0;
	xfer[0].len = 2;
	xfer[0].buf = (uint8_t *)&buf;

    ret = i2c_transfer(i2c->adapter, xfer, 1);
	if (ret < 0)
		return ret;
	else if (ret != 2)
		return -EIO;

    return 0;
}


static const struct regmap_config lt9611_regmap_config = {
    .reg_bits = 8,
    .val_bits = 8,
	.reg_read = i2c_read,
	.reg_write = i2c_write,
    .max_register = 0x100,
};

static int resolution_change_handler(struct notifier_block *nb, unsigned long action, void *data) {
    struct drm_connector *connector = data;
    if (action == DRM_MODE_CONNECTOR_PROPERTY_CHANGE) {
        // 获取新分辨率
        struct drm_display_mode *mode = drm_connector_get_mode(connector);
        printk(KERN_INFO "Resolution changed to %dx%d\n", mode->hdisplay, mode->vdisplay);
    }
    return NOTIFY_OK;
}

static struct notifier_block resolution_notifier = {
    .notifier_call = resolution_change_handler,
};

static int lt9611_i2c_probe(struct i2c_client *i2c, const struct i2c_device_id *id)
{
    struct regmap *regmap;
    unsigned int chip_id[2] = {0};
    int ret;

    regmap = devm_regmap_init(&i2c->dev, NULL, i2c, &lt9611_regmap_config);
    if (IS_ERR(regmap))
    {
        return PTR_ERR(regmap);
    }
    regmap_write(regmap, 0xFF, 0x80);
    regmap_write(regmap, 0xee, 0x01);

    ret = regmap_read(regmap, 0x00, &chip_id[0]);
    if (ret) {
        dev_err(&i2c->dev, "Failed to read chip ID: %d\n", ret);
        return ret;
    }
    ret = regmap_read(regmap, 0x01, &chip_id[1]);
    if (ret) {
        dev_err(&i2c->dev, "Failed to read chip ID: %d\n", ret);
        return ret;
    }

    dev_info(&i2c->dev, "LT9611 Chip ID: 0x%02X 0x%02X\n", chip_id[0], chip_id[1]);

    drm_connector_register(&resolution_notifier);

    return 0;
}

static int lt9611_i2c_remove(struct i2c_client *client)
{
    drm_connector_unregister(&resolution_notifier);
    return 0;
}

static const struct of_device_id lt9611_of_match[] = {
    { .compatible = "lt9611", },
    { }
};

MODULE_DEVICE_TABLE(of, lt9611_of_match);
 
static struct i2c_driver lt9611_driver = {
    .probe = lt9611_i2c_probe,
    .remove = lt9611_i2c_remove,
    .driver = {
        .name = "lt9611",
        .of_match_table = lt9611_of_match,
    },
};

module_i2c_driver(lt9611_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("lt9611 Driver");