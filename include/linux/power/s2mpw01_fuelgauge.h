/*
 * s2mpw01_fuelgauge.h
 * Samsung S2MPW01 Fuel Gauge Header
 *
 * Copyright (C) 2015 Samsung Electronics, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __S2MPW01_FUELGAUGE_H
#define __S2MPW01_FUELGAUGE_H __FILE__
#include <linux/mfd/samsung/s2mpw01.h>
#include <linux/mfd/samsung/s2mpw01-private.h>

#if defined(ANDROID_ALARM_ACTIVATED)
#include <linux/android_alarm.h>
#endif

#include <linux/power/sec_charging_common.h>

/* Slave address should be shifted to the right 1bit.
 * R/W bit should NOT be included.
 */

#define S2MPW01_FG_REG_STATUS           0x00
#define S2MPW01_FG_REG_IRQ              0x02
#define S2MPW01_FG_REG_INTM             0x03
#define S2MPW01_FG_REG_RVBAT            0x04
#define S2MPW01_FG_REG_ROCV             0x06
#define S2MPW01_FG_REG_RSOC_SYS         0x08
#define S2MPW01_FG_REG_RTEMP            0x0A
#define S2MPW01_FG_REG_RBATCAP          0x0C
#define S2MPW01_FG_REG_RZADJ            0x0E
#define S2MPW01_FG_REG_RBATZ0           0x10
#define S2MPW01_FG_REG_RBATZ1           0x12
#define S2MPW01_FG_REG_IRQ_LVL          0x14
#define S2MPW01_FG_REG_CONFIG           0x16

#define S2MPW01_FG_REG_CONFIG_IDLE_STATE        (0x1 << 1)
#define S2MPW01_FG_REG_CONFIG_INIT_STATE        (0x1 << 2)
#define S2MPW01_FG_REG_CONFIG_NORMAL_STATE      (0x1 << 3)

struct sec_fg_info {
	/* test print count */
	int pr_cnt;
	/* full charge comp */
	/* struct delayed_work     full_comp_work; */
	u32 previous_fullcap;
	u32 previous_vffullcap;
	/* low battery comp */
	int low_batt_comp_flag;
	/* low battery boot */
	int low_batt_boot_flag;
	bool is_low_batt_alarm;

	/* battery info */
	u32 soc;

	/* miscellaneous */
	unsigned long fullcap_check_interval;
	int full_check_flag;
	bool is_first_check;
};

typedef struct s2mpw01_fuelgauge_platform_data {
	int capacity_max;
	int capacity_max_margin;
	int capacity_min;
	int capacity_calculation_type;
	int fuel_alert_soc;
	int fullsocthr;
	int fg_irq;

	char *fuelgauge_name;

	bool repeated_fuelalert;

	struct sec_charging_current *charging_current;
} s2mpw01_fuelgauge_platform_data_t;

struct s2mpw01_fuelgauge_data {
	struct device           *dev;
	struct i2c_client       *i2c;
	struct i2c_client       *pmic;

	struct mutex            fuelgauge_mutex;
	struct s2mpw01_fuelgauge_platform_data *pdata;
	struct power_supply	psy_fg;
	/* struct delayed_work isr_work; */

	int cable_type;
	bool is_charging;

	/* HW-dedicated fuel guage info structure
	* used in individual fuel gauge file only
	* (ex. dummy_fuelgauge.c)
	*/
	struct sec_fg_info      info;
	bool is_fuel_alerted;
	struct wake_lock fuel_alert_wake_lock;

	unsigned int capacity_old;      /* only for atomic calculation */
	unsigned int capacity_max;      /* only for dynamic calculation */
	unsigned int standard_capacity;

	bool initial_update_of_soc;
	struct mutex fg_lock;
	struct delayed_work isr_work;

	/* register programming */
	int reg_addr;
	u8 reg_data[2];

	unsigned int pre_soc;
	int fg_irq;
	int before_temp_level;
};

enum {
	TEMP_LEVEL_VERY_LOW = 0,
	TEMP_LEVEL_LOW,
	TEMP_LEVEL_MID,
	TEMP_LEVEL_HIGH,
};
#endif /* __S2MPW01_FUELGAUGE_H */
