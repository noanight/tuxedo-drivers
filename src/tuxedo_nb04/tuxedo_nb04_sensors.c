/*!
 * Copyright (c) 2023 TUXEDO Computers GmbH <tux@tuxedocomputers.com>
 *
 * This file is part of tuxedo-drivers.
 *
 * tuxedo-drivers is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.  If not, see <https://www.gnu.org/licenses/>.
 */
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/module.h>
#include <linux/hwmon.h>
#include <linux/platform_device.h>
#include "tuxedo_nb04_wmi_bs.h"

static const char * const temp_labels[] = {
	"cpu0",
	"gpu0"
};

static const char * const fan_labels[] = {
	"cpu0",
	"gpu0"
};

struct driver_data_t {
	int fan_cpu_max;
	int fan_cpu_min;
	int fan_gpu_max;
	int fan_gpu_min;
};

struct driver_data_t driver_data;

static umode_t
tuxedo_nb04_sensors_is_visible(const void *drvdata, enum hwmon_sensor_types type,
			 u32 attr, int channel)
{
	return 0444;
}

static int
tuxedo_nb04_sensors_read(struct device *dev, enum hwmon_sensor_types type,
		   u32 attr, int channel, long *val)
{
	struct driver_data_t *driver_data = dev_get_drvdata(dev);

	switch (type) {
	case hwmon_temp:
		// TODO: Read from HW
		*val = 15000;
		return 0;
	case hwmon_fan:
		switch (attr) {
		case hwmon_fan_min:
			if (channel == 0) {
				*val = driver_data->fan_cpu_min;
				return 0;
			} else if (channel == 1) {
				*val = driver_data->fan_gpu_min;
				return 0;
			}
			break;
		case hwmon_fan_max:
			if (channel == 0) {
				*val = driver_data->fan_cpu_max;
				return 0;
			} else if (channel == 1) {
				*val = driver_data->fan_gpu_max;
				return 0;
			}
			break;
		case hwmon_fan_input:
			// TODO: Read from HW
			*val = 2400;
			return 0;
		default:
			break;
		}
	default:
		break;
	}

	return -EOPNOTSUPP;
}

static int
tuxedo_nb04_sensors_read_string(struct device *dev, enum hwmon_sensor_types type,
			  u32 attr, int channel, const char **str)
{
	switch (type) {
	case hwmon_temp:
		*str = temp_labels[channel];
		return 0;
	case hwmon_fan:
		*str = fan_labels[channel];
		return 0;
	default:
		break;
	}

	return -EOPNOTSUPP;
}

static const struct hwmon_ops tuxedo_nb04_sensors_ops = {
	.is_visible = tuxedo_nb04_sensors_is_visible,
	.read = tuxedo_nb04_sensors_read,
	.read_string = tuxedo_nb04_sensors_read_string
};

static const struct hwmon_channel_info *const tuxedo_nb04_sensors_info[] = {
	HWMON_CHANNEL_INFO(temp,
			   HWMON_T_INPUT | HWMON_T_LABEL,
			   HWMON_T_INPUT | HWMON_T_LABEL),
	HWMON_CHANNEL_INFO(fan,
			   HWMON_F_INPUT | HWMON_F_LABEL | HWMON_F_MIN | HWMON_F_MAX,
			   HWMON_F_INPUT | HWMON_F_LABEL | HWMON_F_MIN | HWMON_F_MAX),
	NULL
};

static const struct hwmon_chip_info tuxedo_nb04_sensors_chip_info = {
	.ops = &tuxedo_nb04_sensors_ops,
	.info = tuxedo_nb04_sensors_info
};

static int __init tuxedo_nb04_sensors_probe(struct platform_device *pdev) {
	struct device *hwmon_dev;

	driver_data.fan_cpu_max=10000;
	driver_data.fan_cpu_min=0;
	driver_data.fan_gpu_max=10000;
	driver_data.fan_gpu_min=0;

	hwmon_dev = devm_hwmon_device_register_with_info(&pdev->dev,
							 "tuxedo",
							 &driver_data,
							 &tuxedo_nb04_sensors_chip_info,
							 NULL);

	return 0;
}

static struct platform_device *tuxedo_nb04_sensors_device;
static struct platform_driver tuxedo_nb04_sensors_driver = {
	.driver.name = "tuxedo_nb04_sensors",
};

static int __init tuxedo_nb04_sensors_init(void)
{
	tuxedo_nb04_sensors_device =
		platform_create_bundle(&tuxedo_nb04_sensors_driver,
				       tuxedo_nb04_sensors_probe, NULL, 0, NULL, 0);

	if (IS_ERR(tuxedo_nb04_sensors_device))
		return PTR_ERR(tuxedo_nb04_sensors_device);

	return 0;
}

static void __exit tuxedo_nb04_sensors_exit(void)
{
	platform_device_unregister(tuxedo_nb04_sensors_device);
	platform_driver_unregister(&tuxedo_nb04_sensors_driver);
}

module_init(tuxedo_nb04_sensors_init);
module_exit(tuxedo_nb04_sensors_exit);

MODULE_AUTHOR("TUXEDO Computers GmbH <tux@tuxedocomputers.com>");
MODULE_DESCRIPTION("TUXEDO Computers NB04 sensors driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");
MODULE_ALIAS("platform:tuxedo_nb04_wmi_bs");