#include <linux/platform_device.h>

#include "../uniwill_interfaces.h"

#define __unused __attribute__((unused))

static ssize_t ctgp_offset_show(struct device *__unused, struct device_attribute *__unused,
				char *buf)
{
	int result = 0;
	u8 data = 0;

	result = uniwill_read_ec_ram(UW_EC_REG_CTGP_DB_CTGP_OFFSET, &data);
	if (result < 0)
		return result;

	return sysfs_emit(buf, "%u\n", data);
}
static ssize_t ctgp_offset_store(struct device *__unused, struct device_attribute *__unused,
				 const char *buf, size_t count)
{
	int result = 0;
	u8 data = 0;

	result = kstrtou8(buf, 0, &data);
	if (result < 0)
		return result;

	result = uniwill_write_ec_ram(UW_EC_REG_CTGP_DB_CTGP_OFFSET, data);
	if (result < 0)
		return result;

	return count;
}
DEVICE_ATTR_RW(ctgp_offset);

static u8 max_ctgp_offset = 25;
static ssize_t max_ctgp_offset_show(struct device *__unused, struct device_attribute *__unused,
				    char *buf)
{
	return sysfs_emit(buf, "%u\n", max_ctgp_offset);
}
DEVICE_ATTR_RO(max_ctgp_offset);

static ssize_t db_offset_show(struct device *__unused, struct device_attribute *__unused, char *buf)
{
	int result = 0;
	u8 data = 0;

	result = uniwill_read_ec_ram(UW_EC_REG_CTGP_DB_DB_OFFSET, &data);
	if (result < 0)
		return result;

	return sysfs_emit(buf, "%u\n", data);
}
static ssize_t db_offset_store(struct device *__unused, struct device_attribute *__unused,
			       const char *buf, size_t count)
{
	int result = 0;
	u8 data = 0;

	result = kstrtou8(buf, 0, &data);
	if (result < 0)
		return result;

	result = uniwill_write_ec_ram(UW_EC_REG_CTGP_DB_DB_OFFSET, data);
	if (result < 0)
		return result;

	return count;
}
DEVICE_ATTR_RW(db_offset);

static u8 max_db_offset = 25;
static ssize_t max_db_offset_show(struct device *__unused, struct device_attribute *__unused,
				  char *buf)
{
	return sysfs_emit(buf, "%u\n", max_db_offset);
}
DEVICE_ATTR_RO(max_db_offset);

static u8 max_combined_offset = 50;
static ssize_t max_combined_offset_show(struct device *__unused, struct device_attribute *__unused,
				  char *buf)
{
	return sysfs_emit(buf, "%u\n", max_combined_offset);
}
DEVICE_ATTR_RO(max_combined_offset);

static int __init init_db_and_ctgp(void)
{
	int result = 0;

	// TODO Get max cTGP and DB offsets

	result = uniwill_write_ec_ram(UW_EC_REG_FAN_CTRL_STATUS,
				      UW_EC_REG_CTGP_DB_ENABLE_BIT_GENERAL_ENABLE |
				      UW_EC_REG_CTGP_DB_ENABLE_BIT_DB_ENABLE |
				      UW_EC_REG_CTGP_DB_ENABLE_BIT_CTGP_ENABLE);
	if (result < 0)
		return result;

	result = uniwill_write_ec_ram(UW_EC_REG_CTGP_DB_CTGP_OFFSET, 0);
	if (result < 0)
		return result;

	result = uniwill_write_ec_ram(UW_EC_REG_CTGP_DB_MYSTERY_OFFSET, 35);
	if (result < 0)
		return result;

	result = uniwill_write_ec_ram(UW_EC_REG_CTGP_DB_DB_OFFSET, 5);
	if (result < 0)
		return result;

	return 0;
}

static int __init init_sysfs_attrs(struct platform_device *pdev)
{
	int result = 0;

	result = sysfs_create_file(&pdev->dev.kobj, &dev_attr_ctgp_offset.attr);
	if (result)
		return result;

	result = sysfs_create_file(&pdev->dev.kobj, &dev_attr_max_ctgp_offset.attr);
	if (result)
		return result;

	result = sysfs_create_file(&pdev->dev.kobj, &dev_attr_db_offset.attr);
	if (result)
		return result;

	result = sysfs_create_file(&pdev->dev.kobj, &dev_attr_max_db_offset.attr);
	if (result)
		return result;

	result = sysfs_create_file(&pdev->dev.kobj, &dev_attr_max_combined_offset.attr);
	if (result)
		return result;

	return 0;
}

static int __init tuxedo_nb02_nvidia_power_ctrl_probe(struct platform_device *pdev) {
	int result = 0;
	u8 data = 0;
	char **uniwill_active_interface = NULL;

	result = uniwill_get_active_interface_id(uniwill_active_interface);
	if (result < 0)
		return result;

	// TODO Replace by check for NVIDIA 3000 series or newer
	result = uniwill_read_ec_ram(UW_EC_REG_FAN_CTRL_STATUS, &data);
	if (result < 0)
		return result;
	if (!(data >> UW_EC_REG_FAN_CTRL_STATUS_BIT_HAS_UW_FAN_CTRL) & 0x01)
		return -ENODEV;

	result = init_db_and_ctgp();
	if (result < 0)
		return result;

	result = init_sysfs_attrs(pdev);
	if (result < 0)
		return result;

	return 0;
}



// Boilerplate

static struct platform_device *tuxedo_nb02_nvidia_power_ctrl_device;
static struct platform_driver tuxedo_nb02_nvidia_power_ctrl_driver = {
	.driver.name = "tuxedo_nb02_nvidia_power_ctrl",
};

static int __init tuxedo_nb02_nvidia_power_ctrl_init(void)
{
	tuxedo_nb02_nvidia_power_ctrl_device =
		platform_create_bundle(&tuxedo_nb02_nvidia_power_ctrl_driver,
				       tuxedo_nb02_nvidia_power_ctrl_probe, NULL, 0, NULL, 0);

	if (IS_ERR(tuxedo_nb02_nvidia_power_ctrl_device))
		return PTR_ERR(tuxedo_nb02_nvidia_power_ctrl_device);

	return 0;
}

static void __exit tuxedo_nb02_nvidia_power_ctrl_exit(void)
{
	platform_device_unregister(tuxedo_nb02_nvidia_power_ctrl_device);
	platform_driver_unregister(&tuxedo_nb02_nvidia_power_ctrl_driver);
}

module_init(tuxedo_nb02_nvidia_power_ctrl_init);
module_exit(tuxedo_nb02_nvidia_power_ctrl_exit);

MODULE_AUTHOR("TUXEDO Computers GmbH <tux@tuxedocomputers.com>");
MODULE_DESCRIPTION("TUXEDO Computers Dynamic Boost and cTGP control driver for NVIDIA silicon for devices marked with board_vendor NB02");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");
