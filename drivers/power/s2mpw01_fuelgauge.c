/*
 *  s2mpw01_fuelgauge.c
 *  Samsung S2MPW01 Fuel Gauge Driver
 *
 *  Copyright (C) 2015 Samsung Electronics
 *  Developed by Nguyen Tien Dat (tiendat.nt@samsung.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#define DEBUG

#define SINGLE_BYTE	1

#include <linux/power/s2mpw01_fuelgauge.h>
#include <linux/of_gpio.h>
#include <linux/mfd/samsung/s2mpw01.h>

#ifdef CONFIG_ARCH_SWA100
#define SEC_BATTERY_VOLTAGE_AVERAGE (SEC_BATTEY_VOLTAGE_AVERAGE)
#define SEC_BATTERY_VOLTAGE_OCV (SEC_BATTEY_VOLTAGE_OCV)
#endif

static enum power_supply_property s2mpw01_fuelgauge_props[] = {
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_VOLTAGE_AVG,
	POWER_SUPPLY_PROP_VOLTAGE_OCV,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_TEMP,
	POWER_SUPPLY_PROP_TEMP_AMBIENT,
	POWER_SUPPLY_PROP_ENERGY_FULL_DESIGN,
	POWER_SUPPLY_PROP_CHARGE_TYPE,
};


static int s2mpw01_fg_write_reg_byte(struct i2c_client *client, int reg, u8 data)
{
	int ret, i = 0;

	ret = s2mpw01_write_reg(client, reg,  data);
	if (ret < 0) {
		for (i = 0; i < 3; i++) {
			ret = s2mpw01_write_reg(client, reg,  data);
			if (ret >= 0)
				break;
		}

		if (i >= 3)
			dev_err(&client->dev, "%s: Error(%d)\n", __func__, ret);
	}

	return ret;
}

static int s2mpw01_fg_write_reg(struct i2c_client *client, int reg, u8 *buf)
{
#if SINGLE_BYTE
	int ret = 0;
	s2mpw01_fg_write_reg_byte(client, reg, buf[0]);
	s2mpw01_fg_write_reg_byte(client, reg+1, buf[1]);
#else
	int ret, i = 0;

	ret = s2mpw01_bulk_write(client, reg, 2, buf);
	if (ret < 0) {
		for (i = 0; i < 3; i++) {
			ret = s2mpw01_bulk_write(client, reg, 2, buf);
			if (ret >= 0)
				break;
		}

		if (i >= 3)
			dev_err(&client->dev, "%s: Error(%d)\n", __func__, ret);
	}
#endif
	return ret;
}

static int s2mpw01_fg_read_reg_byte(struct i2c_client *client, int reg, void *data)
{
	int ret;
	u8 temp = 0;

	ret = s2mpw01_read_reg(client, reg, &temp);
	if (ret < 0)
		return ret;
	*(u8 *)data = (u8)temp;

	return ret;
}

static int s2mpw01_fg_read_reg(struct i2c_client *client, int reg, u8 *buf)
{
#if SINGLE_BYTE
	int ret = 0;
	u8 data1 = 0, data2 = 0;
	s2mpw01_fg_read_reg_byte(client, reg, &data1);
	s2mpw01_fg_read_reg_byte(client, reg + 1, &data2);
	buf[0] = data1;
	buf[1] = data2;
#else
	int ret = 0, i = 0;

	ret = s2mpw01_bulk_read(client, reg, 2, buf);
	if (ret < 0) {
		for (i = 0; i < 3; i++) {
			ret = s2mpw01_bulk_read(client, reg, 2, buf);
			if (ret >= 0)
				break;
		}

		if (i >= 3)
			dev_err(&client->dev, "%s: Error(%d)\n", __func__, ret);
	}
#endif
	return ret;
}

static int s2mpw01_init_regs(struct s2mpw01_fuelgauge_data *fuelgauge)
{
	bool ret = false;
	u8 data[2];
	int i;

	pr_info("%s: s2mpw01 fuelgauge initialize\n", __func__);

	/* check if Smn was generated */
	if (s2mpw01_fg_read_reg(fuelgauge->i2c, S2MPW01_FG_REG_STATUS, data) < 0)
		return ret;

	if (data[0] & S2MPW01_FG_REG_CONFIG_IDLE_STATE) {
		pr_info("%s: idle state, start it.\n", __func__);
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, S2MPW01_FG_REG_CONFIG, 0x03);
	} else if (data[0] & S2MPW01_FG_REG_CONFIG_INIT_STATE) {
		pr_info("%s: init state.\n", __func__);
	} else if (data[0] & S2MPW01_FG_REG_CONFIG_NORMAL_STATE) {
		pr_info("%s: normal state, has been initialized.\n", __func__);
		return true;
	}

	for (i = 0; i < 50; i++) {
		if (s2mpw01_fg_read_reg(fuelgauge->i2c, S2MPW01_FG_REG_STATUS, data) < 0)
			return ret;

		if (data[0] & S2MPW01_FG_REG_CONFIG_NORMAL_STATE) {
			pr_info("%s: enters normal state, initialize done: %d.\n", __func__, i);
			ret = true;
			break;
		}
		mdelay(1);
	}

	if (ret == false) {
		pr_err("%s: FG initialize failed: %d%d.\n", __func__, data[1], data[0]);
	}
	return ret;
}

static void s2mpw01_alert_init(struct s2mpw01_fuelgauge_data *fuelgauge)
{
	u8 data[2];

	/* VBAT Threshold setting: 3.55V */
	data[0] = 0x00 & 0x0f;

	/* SOC Threshold setting */
	data[0] = data[0] | (fuelgauge->pdata->fuel_alert_soc << 4);

	data[1] = 0x00;
	s2mpw01_fg_write_reg(fuelgauge->i2c, S2MPW01_FG_REG_IRQ_LVL, data);
}

static bool s2mpw01_check_status(struct i2c_client *client)
{
	u8 data[2];
	bool ret = false;

	/* check if Smn was generated */
	if (s2mpw01_fg_read_reg(client, S2MPW01_FG_REG_STATUS, data) < 0)
		return ret;

	dev_info(&client->dev, "%s: status to (%02x%02x)\n",
		__func__, data[1], data[0]);

	if (data[1] & (0x1 << 1))
		return true;
	else
		return false;
}

static int s2mpw01_set_temperature(struct s2mpw01_fuelgauge_data *fuelgauge,
			int temperature)
{
	char val;
	//u8 temp1, temp2, temp3;
	int temperature_level;

	val = temperature / 10;

	if (val < 0)
		temperature_level = TEMP_LEVEL_VERY_LOW;
	else if (val < 15)
		temperature_level = TEMP_LEVEL_LOW;
	else if (val > 35)
		temperature_level = TEMP_LEVEL_HIGH;
	else
		temperature_level = TEMP_LEVEL_MID;

	if (fuelgauge->before_temp_level == temperature_level)
		return temperature;

	fuelgauge->before_temp_level = temperature_level;
#if 0
	s2mpw01_fg_read_reg_byte(fuelgauge->i2c, 0x19, &temp1);
	s2mpw01_fg_read_reg_byte(fuelgauge->i2c, 0x28, &temp2);
	s2mpw01_fg_read_reg_byte(fuelgauge->i2c, 0x2B, &temp3);
	temp1 &= 0x0F;
	temp2 &= 0x0F;
	temp3 &= 0x0F;

	if (val < 15) {
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, 0x0B, 0xE0);
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, 0x0E, 0x4A);
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, 0x0F, 0x36);
		temp1 |= 0xE0;
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, 0x19, temp1);
		temp2 |= 0x10;
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, 0x28, temp2);
		temp3 |= 0xF0;
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, 0x2B, temp3);
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, S2MPW01_FG_REG_RTEMP, 10);
	} else if (val >= 15 && val <= 35) {
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, 0x0B, 0x20);
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, 0x0E, 0x76);
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, 0x0F, 0xAE);
		temp1 |= 0xF0;
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, 0x19, temp1);
		temp2 |= 0x00;
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, 0x28, temp2);
		temp3 |= 0x00;
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, 0x2B, temp3);
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, S2MPW01_FG_REG_RTEMP, 25);
	} else if (val > 35) {
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, 0x0B, 0x20);
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, 0x0E, 0x9A);
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, 0x0F, 0xA0);
		temp1 |= 0xF0;
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, 0x19, temp1);
		temp2 |= 0x00;
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, 0x28, temp2);
		temp3 |= 0x00;
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, 0x2B, temp3);
		s2mpw01_fg_write_reg_byte(fuelgauge->i2c, S2MPW01_FG_REG_RTEMP, 40);
	}
#endif
	dev_dbg(&fuelgauge->i2c->dev, "%s: temperature to (%d)\n",
		__func__, temperature);

	return temperature;
}

static int s2mpw01_get_temperature(struct s2mpw01_fuelgauge_data *fuelgauge)
{
	u8 data[2];
	s32 temperature = 0;

	/*
	 *  use monitor regiser.
	 *  monitor register default setting is temperature
	 */
	if (s2mpw01_fg_read_reg(fuelgauge->i2c, S2MPW01_FG_REG_RTEMP, data) < 0)
		return -ERANGE;

	/* data[] store 2's compliment format number */
	if (data[0] & (0x1 << 7)) {
		/* Negative */
		temperature = ((~(data[0])) & 0xFF) + 1;
		temperature *= -10;
	} else {
		temperature = data[0] & 0x7F;
		temperature *= 10;
	}

	dev_info(&fuelgauge->i2c->dev, "%s: temperature (%d)\n",
		__func__, temperature);

	return temperature;
}

#if 0
/* soc should be 0.01% unit */
static int s2mpw01_get_soc(struct s2mpw01_fuelgauge_data *fuelgauge)
{
	u8 data[2], check_data[2];
	u16 compliment;
	int rsoc, i;

	for (i = 0; i < 50; i++) {
		if (s2mpw01_fg_read_reg(fuelgauge->i2c, S2MPW01_FG_REG_RSOC, data) < 0)
			return -EINVAL;
		if (s2mpw01_fg_read_reg(fuelgauge->i2c, S2MPW01_FG_REG_RSOC, check_data) < 0)
			return -EINVAL;
	dev_dbg(&fuelgauge->i2c->dev, "[DEBUG]%s: data0 (%d) data1 (%d)\n", __func__, data[0], data[1]);

	if ((data[0] == check_data[0]) && (data[1] == check_data[1]))
			break;
	}

	dev_dbg(&fuelgauge->i2c->dev, "[DEBUG]%s: data0 (%d) data1 (%d)\n", __func__, data[0], data[1]);
	compliment = (data[1] << 8) | (data[0]);

	/* data[] store 2's compliment format number */
	if (compliment & (0x1 << 15)) {
		/* Negative */
		rsoc = ((~compliment) & 0xFFFF) + 1;
		rsoc = (rsoc * (-10000)) / (0x1 << 12);
	} else {
		rsoc = compliment & 0x7FFF;
		rsoc = ((rsoc * 10000) / (0x1 << 12));
	}

	dev_dbg(&fuelgauge->i2c->dev, "[DEBUG]%s: raw capacity (0x%x:%d)\n", __func__,
		compliment, rsoc);

	return min(rsoc, 10000) / 10;
}
#endif
static int s2mpw01_get_rawsoc(struct s2mpw01_fuelgauge_data *fuelgauge)
{
	u8 data[2], check_data[2];
	u16 compliment;
	u32 bat_cap;
	int rsoc, i;

	for (i = 0; i < 50; i++) {
		if (s2mpw01_fg_read_reg(fuelgauge->i2c, S2MPW01_FG_REG_RSOC_SYS, data) < 0)
			return -EINVAL;
		if (s2mpw01_fg_read_reg(fuelgauge->i2c, S2MPW01_FG_REG_RSOC_SYS, check_data) < 0)
			return -EINVAL;
		if ((data[0] == check_data[0]) && (data[1] == check_data[1]))
			break;
	}

	compliment = (data[1] << 8) | (data[0]);

	/* data[] store 2's compliment format number */
	if (compliment & (0x1 << 15)) {
		/* Negative */
		rsoc = ((~compliment) & 0xFFFF) + 1;
		rsoc = (rsoc * (-10000)) / (0x1 << 12);
	} else {
		rsoc = compliment & 0x7FFF;
		rsoc = ((rsoc * 10000) / (0x1 << 12));
	}

	s2mpw01_fg_read_reg(fuelgauge->i2c, S2MPW01_FG_REG_RBATCAP, data);

	bat_cap = ((data[1] & 0x3F) << 8) | data[0];
	dev_info(&fuelgauge->i2c->dev, "[DEBUG]%s: raw capacity (0x%x:%d), batcap: (0x%x:%d) \n", __func__,
			compliment, rsoc, bat_cap, bat_cap);
	return min(rsoc/10, 1000);
}

static int s2mpw01_get_ocv(struct s2mpw01_fuelgauge_data *fuelgauge)
{
	u8 data[2];
	u32 rocv = 0;

	if (s2mpw01_fg_read_reg(fuelgauge->i2c, S2MPW01_FG_REG_ROCV, data) < 0)
		return -EINVAL;

	rocv = ((data[0] + (data[1] << 8)) * 1000) >> 13;

	dev_dbg(&fuelgauge->i2c->dev, "%s: rocv (%d)\n", __func__, rocv);


	return rocv;
}

static int s2mpw01_get_vbat(struct s2mpw01_fuelgauge_data *fuelgauge)
{
	u8 data[2];
	u32 vbat = 0;

	if (s2mpw01_fg_read_reg(fuelgauge->i2c, S2MPW01_FG_REG_RVBAT, data) < 0)
		return -EINVAL;

	dev_dbg(&fuelgauge->i2c->dev, "%s: data0 (%d) data1 (%d)\n", __func__, data[0], data[1]);
	vbat = ((data[0] + (data[1] << 8)) * 1000) >> 13;

	dev_info(&fuelgauge->i2c->dev, "%s: vbat (%d)\n", __func__, vbat);

	return vbat;
}

static int s2mpw01_get_avgvbat(struct s2mpw01_fuelgauge_data *fuelgauge)
{
	u8 data[2];
	u32 new_vbat, old_vbat = 0;
	int cnt;

	for (cnt = 0; cnt < 5; cnt++) {
		if (s2mpw01_fg_read_reg(fuelgauge->i2c, S2MPW01_FG_REG_RVBAT, data) < 0)
			return -EINVAL;

		new_vbat = ((data[0] + (data[1] << 8)) * 1000) >> 13;

		if (cnt == 0)
			old_vbat = new_vbat;
		else
			old_vbat = new_vbat / 2 + old_vbat / 2;
	}

	dev_info(&fuelgauge->i2c->dev, "%s: avgvbat (%d)\n", __func__, old_vbat);

	return old_vbat;
}

/* capacity is  0.1% unit */
static void s2mpw01_fg_get_scaled_capacity(
		struct s2mpw01_fuelgauge_data *fuelgauge,
		union power_supply_propval *val)
{
	val->intval = (val->intval < fuelgauge->pdata->capacity_min) ?
		0 : ((val->intval - fuelgauge->pdata->capacity_min) * 1000 /
		(fuelgauge->capacity_max - fuelgauge->pdata->capacity_min));

	dev_info(&fuelgauge->i2c->dev,
			"%s: scaled capacity (%d.%d)\n",
			__func__, val->intval/10, val->intval%10);
}

/* capacity is integer */
static void s2mpw01_fg_get_atomic_capacity(
		struct s2mpw01_fuelgauge_data *fuelgauge,
		union power_supply_propval *val)
{
	if (fuelgauge->pdata->capacity_calculation_type &
			SEC_FUELGAUGE_CAPACITY_TYPE_ATOMIC) {
		if (fuelgauge->capacity_old < val->intval)
			val->intval = fuelgauge->capacity_old + 1;
		else if (fuelgauge->capacity_old > val->intval)
			val->intval = fuelgauge->capacity_old - 1;
	}

	/* keep SOC stable in abnormal status */
	if (fuelgauge->pdata->capacity_calculation_type &
			SEC_FUELGAUGE_CAPACITY_TYPE_SKIP_ABNORMAL) {
		if (!fuelgauge->is_charging &&
				fuelgauge->capacity_old < val->intval) {
			dev_err(&fuelgauge->i2c->dev,
					"%s: capacity (old %d : new %d)\n",
					__func__, fuelgauge->capacity_old, val->intval);
			val->intval = fuelgauge->capacity_old;
		}
	}

	/* updated old capacity */
	fuelgauge->capacity_old = val->intval;
}

static int s2mpw01_fg_check_capacity_max(
		struct s2mpw01_fuelgauge_data *fuelgauge, int capacity_max)
{
	int new_capacity_max = capacity_max;

	if (new_capacity_max < (fuelgauge->pdata->capacity_max -
				fuelgauge->pdata->capacity_max_margin - 10)) {
		new_capacity_max =
			(fuelgauge->pdata->capacity_max -
			 fuelgauge->pdata->capacity_max_margin);

		dev_info(&fuelgauge->i2c->dev, "%s: set capacity max(%d --> %d)\n",
				__func__, capacity_max, new_capacity_max);
	} else if (new_capacity_max > (fuelgauge->pdata->capacity_max +
				fuelgauge->pdata->capacity_max_margin)) {
		new_capacity_max =
			(fuelgauge->pdata->capacity_max +
			 fuelgauge->pdata->capacity_max_margin);

		dev_info(&fuelgauge->i2c->dev, "%s: set capacity max(%d --> %d)\n",
				__func__, capacity_max, new_capacity_max);
	}

	return new_capacity_max;
}

static int s2mpw01_fg_calculate_dynamic_scale(
		struct s2mpw01_fuelgauge_data *fuelgauge, int capacity)
{
	union power_supply_propval raw_soc_val;
	raw_soc_val.intval = s2mpw01_get_rawsoc(fuelgauge) / 10;

	if (raw_soc_val.intval <
			fuelgauge->pdata->capacity_max -
			fuelgauge->pdata->capacity_max_margin) {
		fuelgauge->capacity_max =
			fuelgauge->pdata->capacity_max -
			fuelgauge->pdata->capacity_max_margin;
		dev_dbg(&fuelgauge->i2c->dev, "%s: capacity_max (%d)",
				__func__, fuelgauge->capacity_max);
	} else {
		fuelgauge->capacity_max =
			(raw_soc_val.intval >
			 fuelgauge->pdata->capacity_max +
			 fuelgauge->pdata->capacity_max_margin) ?
			(fuelgauge->pdata->capacity_max +
			 fuelgauge->pdata->capacity_max_margin) :
			raw_soc_val.intval;
		dev_dbg(&fuelgauge->i2c->dev, "%s: raw soc (%d)",
				__func__, fuelgauge->capacity_max);
	}

	if (capacity != 100) {
		fuelgauge->capacity_max = s2mpw01_fg_check_capacity_max(
			fuelgauge, (fuelgauge->capacity_max * 100 / capacity));
	} else  {
		fuelgauge->capacity_max =
			(fuelgauge->capacity_max * 99 / 100);
	}

	/* update capacity_old for sec_fg_get_atomic_capacity algorithm */
	fuelgauge->capacity_old = capacity;

	dev_info(&fuelgauge->i2c->dev, "%s: %d is used for capacity_max\n",
			__func__, fuelgauge->capacity_max);

	return fuelgauge->capacity_max;
}

bool s2mpw01_fuelgauge_fuelalert_init(struct s2mpw01_fuelgauge_data *fuelgauge , int soc)
{
	u8 data[2];

	/* 1. Set s2mpw01 alert configuration. */
	s2mpw01_alert_init(fuelgauge);

	if (s2mpw01_fg_read_reg(fuelgauge->i2c, S2MPW01_FG_REG_IRQ, data) < 0)
		return -1;

	/*Enable VBAT, SOC */
	data[1] &= 0xfc;

	/*Disable IDLE_ST, INIT_ST */
	data[1] |= 0x0c;

	s2mpw01_fg_write_reg(fuelgauge->i2c, S2MPW01_FG_REG_IRQ, data);

	dev_dbg(&fuelgauge->i2c->dev, "%s: irq_reg(%02x%02x) irq(%d)\n",
			__func__, data[1], data[0], fuelgauge->pdata->fg_irq);

	return true;
}

bool s2mpw01_fuelgauge_is_fuelalerted(struct s2mpw01_fuelgauge_data *fuelgauge)
{
	return s2mpw01_check_status(fuelgauge->i2c);
}

bool s2mpw01_hal_fg_fuelalert_process(void *irq_data, bool is_fuel_alerted)
{
	struct s2mpw01_fuelgauge_data *fuelgauge = irq_data;
	int ret;

	ret = s2mpw01_fg_write_reg_byte(fuelgauge->i2c, S2MPW01_FG_REG_IRQ, 0x00);
	if (ret < 0)
		dev_err(&fuelgauge->i2c->dev, "%s: Error(%d)\n", __func__, ret);

	return ret;
}

bool s2mpw01_hal_fg_full_charged(struct i2c_client *client)
{
	return true;
}

static int s2mpw01_fg_get_property(struct power_supply *psy,
		enum power_supply_property psp,
		union power_supply_propval *val)
{
	struct s2mpw01_fuelgauge_data *fuelgauge =
		container_of(psy, struct s2mpw01_fuelgauge_data, psy_fg);

	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
	case POWER_SUPPLY_PROP_CHARGE_FULL:
	case POWER_SUPPLY_PROP_ENERGY_NOW:
		return -ENODATA;
		/* Cell voltage (VCELL, mV) */
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		val->intval = s2mpw01_get_vbat(fuelgauge);
		break;
		/* Additional Voltage Information (mV) */
	case POWER_SUPPLY_PROP_VOLTAGE_AVG:
		val->intval = s2mpw01_get_avgvbat(fuelgauge);
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_OCV:
		val->intval = s2mpw01_get_ocv(fuelgauge);
		break;
		/* Current (mA) */
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		val->intval = 0;
		/* val->intval = s2mpw01_get_current(fuelgauge); */
		break;
		/* Average Current (mA) */
	case POWER_SUPPLY_PROP_CURRENT_AVG:
		val->intval = 0;
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		val->intval = s2mpw01_get_rawsoc(fuelgauge);

		if (fuelgauge->pdata->capacity_calculation_type &
			(SEC_FUELGAUGE_CAPACITY_TYPE_SCALE |
				SEC_FUELGAUGE_CAPACITY_TYPE_DYNAMIC_SCALE))
			s2mpw01_fg_get_scaled_capacity(fuelgauge, val);

		/* capacity should be between 0% and 100%
		 * (0.1% degree)
		 */
		if (val->intval > 1000)
			val->intval = 1000;
		if (val->intval < 0)
			val->intval = 0;

		/* get only integer part */
		val->intval /= 10;

		/* check whether doing the wake_unlock */
		if ((val->intval > fuelgauge->pdata->fuel_alert_soc) &&
				fuelgauge->is_fuel_alerted) {
			wake_unlock(&fuelgauge->fuel_alert_wake_lock);
			s2mpw01_fuelgauge_fuelalert_init(fuelgauge,
					fuelgauge->pdata->fuel_alert_soc);
		}

		/* (Only for atomic capacity)
		 * In initial time, capacity_old is 0.
		 * and in resume from sleep,
		 * capacity_old is too different from actual soc.
		 * should update capacity_old
		 * by val->intval in booting or resume.
		 */
		if (fuelgauge->initial_update_of_soc) {
			/* updated old capacity */
			fuelgauge->capacity_old = val->intval;
			fuelgauge->initial_update_of_soc = false;
			break;
		}

		if (fuelgauge->pdata->capacity_calculation_type &
			(SEC_FUELGAUGE_CAPACITY_TYPE_ATOMIC |
				 SEC_FUELGAUGE_CAPACITY_TYPE_SKIP_ABNORMAL))
			s2mpw01_fg_get_atomic_capacity(fuelgauge, val);

		break;
	/* Battery Temperature */
	case POWER_SUPPLY_PROP_TEMP:
	/* Target Temperature */
	case POWER_SUPPLY_PROP_TEMP_AMBIENT:
		val->intval = s2mpw01_get_temperature(fuelgauge);
		break;
	case POWER_SUPPLY_PROP_ENERGY_FULL_DESIGN:
		val->intval = fuelgauge->capacity_max;
		break;
#if defined(CONFIG_ARCH_SWA100)
	case POWER_SUPPLY_PROP_CHARGE_TYPE:
		val->intval = 0;
		break;
#endif
	default:
		return -EINVAL;
	}

	return 0;
}

static int s2mpw01_fg_set_property(struct power_supply *psy,
	enum power_supply_property psp, const union power_supply_propval *val)
{
	struct s2mpw01_fuelgauge_data *fuelgauge =
		container_of(psy, struct s2mpw01_fuelgauge_data, psy_fg);

	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		break;
	case POWER_SUPPLY_PROP_CHARGE_FULL:
		if (fuelgauge->pdata->capacity_calculation_type &
				SEC_FUELGAUGE_CAPACITY_TYPE_DYNAMIC_SCALE) {
#if defined(CONFIG_PREVENT_SOC_JUMP)
			s2mpw01_fg_calculate_dynamic_scale(fuelgauge, val->intval);
#else
			s2mpw01_fg_calculate_dynamic_scale(fuelgauge, 100);
#endif
		}
		break;
	case POWER_SUPPLY_PROP_ONLINE:
		fuelgauge->cable_type = val->intval;
		if (val->intval == POWER_SUPPLY_TYPE_BATTERY)
			fuelgauge->is_charging = false;
		else
			fuelgauge->is_charging = true;
	case POWER_SUPPLY_PROP_CAPACITY:
		if (val->intval == SEC_FUELGAUGE_CAPACITY_TYPE_RESET)
			fuelgauge->initial_update_of_soc = true;
		break;
	case POWER_SUPPLY_PROP_TEMP:
	case POWER_SUPPLY_PROP_TEMP_AMBIENT:
		s2mpw01_set_temperature(fuelgauge, val->intval);
		break;
	case POWER_SUPPLY_PROP_ENERGY_NOW:
		do {
			u8 temp = 0;
			s2mpw01_fg_read_reg_byte(fuelgauge->i2c, 0x27, &temp);
			if (val->intval)
				temp |= 0x80;
			else
				temp &= ~0x80;

			s2mpw01_fg_write_reg_byte(fuelgauge->i2c, 0x27, temp);
		} while (0);
		break;
	case POWER_SUPPLY_PROP_ENERGY_FULL_DESIGN:
		dev_info(&fuelgauge->i2c->dev,
			"%s: capacity_max changed, %d -> %d\n",
			__func__, fuelgauge->capacity_max, val->intval);
		fuelgauge->capacity_max = s2mpw01_fg_check_capacity_max(fuelgauge, val->intval);
		fuelgauge->initial_update_of_soc = true;
		break;
	case POWER_SUPPLY_PROP_CHARGE_TYPE:
		/* rt5033_fg_reset_capacity_by_jig_connection(fuelgauge->i2c); */
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static void s2mpw01_fg_isr_work(struct work_struct *work)
{
	struct s2mpw01_fuelgauge_data *fuelgauge =
		container_of(work, struct s2mpw01_fuelgauge_data, isr_work.work);
	u8 fg_alert_status;

	s2mpw01_fg_read_reg_byte(fuelgauge->i2c, S2MPW01_FG_REG_STATUS+1, &fg_alert_status);
	dev_info(&fuelgauge->i2c->dev, "%s : fg_alert_status(0x%x)\n",
		__func__, fg_alert_status);

	fg_alert_status &= 0x03;
	if (fg_alert_status & 0x01)
		pr_info("%s : Battery Voltage is very Low!\n", __func__);

	if (fg_alert_status & 0x02)
		pr_info("%s : Battery Level is Very Low!\n", __func__);

	if (!fg_alert_status) {
		pr_info("%s : SOC or Volage is Good!\n", __func__);
		wake_unlock(&fuelgauge->fuel_alert_wake_lock);
	}
}

static irqreturn_t s2mpw01_fg_irq_thread(int irq, void *irq_data)
{
	struct s2mpw01_fuelgauge_data *fuelgauge = irq_data;
	u8 fg_irq = 0;

	s2mpw01_fg_read_reg_byte(fuelgauge->i2c, S2MPW01_FG_REG_IRQ, &fg_irq);
	dev_info(&fuelgauge->i2c->dev, "%s: fg_irq(0x%x)\n",
		__func__, fg_irq);
	wake_lock(&fuelgauge->fuel_alert_wake_lock);
	schedule_delayed_work(&fuelgauge->isr_work, 0);

	return IRQ_HANDLED;
}

#ifdef CONFIG_OF
static int s2mpw01_fuelgauge_parse_dt(struct s2mpw01_fuelgauge_data *fuelgauge)
{
	struct device_node *np = of_find_node_by_name(NULL, "s2mpw01-fuelgauge");
	int ret;
	int i, len;
	const u32 *p;

	/* reset, irq gpio info */
	if (np == NULL) {
		pr_err("%s np NULL\n", __func__);
	} else {
		ret = of_property_read_u32(np, "fuelgauge,capacity_max",
				&fuelgauge->pdata->capacity_max);
		if (ret < 0)
			pr_err("%s error reading capacity_max %d\n", __func__, ret);

		ret = of_property_read_u32(np, "fuelgauge,capacity_max_margin",
				&fuelgauge->pdata->capacity_max_margin);
		if (ret < 0)
			pr_err("%s error reading capacity_max_margin %d\n", __func__, ret);

		ret = of_property_read_u32(np, "fuelgauge,capacity_min",
				&fuelgauge->pdata->capacity_min);
		if (ret < 0)
			pr_err("%s error reading capacity_min %d\n", __func__, ret);

		ret = of_property_read_u32(np, "fuelgauge,capacity_calculation_type",
				&fuelgauge->pdata->capacity_calculation_type);
		if (ret < 0)
			pr_err("%s error reading capacity_calculation_type %d\n",
					__func__, ret);

		ret = of_property_read_u32(np, "fuelgauge,fuel_alert_soc",
				&fuelgauge->pdata->fuel_alert_soc);
		if (ret < 0)
			pr_err("%s error reading pdata->fuel_alert_soc %d\n",
					__func__, ret);
		fuelgauge->pdata->repeated_fuelalert = of_property_read_bool(np,
				"fuelgauge,repeated_fuelalert");

		np = of_find_node_by_name(NULL, "battery");
		if (!np) {
			pr_err("%s np NULL\n", __func__);
		} else {
			ret = of_property_read_string(np,
				"battery,fuelgauge_name",
				(char const **)&fuelgauge->pdata->fuelgauge_name);
			p = of_get_property(np,
					"battery,input_current_limit", &len);
			if (!p)
				return 1;

			len = len / sizeof(u32);
			fuelgauge->pdata->charging_current =
					kzalloc(sizeof(struct sec_charging_current) * len,
					GFP_KERNEL);

			for (i = 0; i < len; i++) {
				ret = of_property_read_u32_index(np,
					"battery,input_current_limit", i,
					&fuelgauge->pdata->charging_current[i].input_current_limit);
				ret = of_property_read_u32_index(np,
					"battery,fast_charging_current", i,
					&fuelgauge->pdata->charging_current[i].fast_charging_current);
				ret = of_property_read_u32_index(np,
					"battery,full_check_current_1st", i,
					&fuelgauge->pdata->charging_current[i].full_check_current_1st);
				ret = of_property_read_u32_index(np,
					"battery,full_check_current_2nd", i,
					&fuelgauge->pdata->charging_current[i].full_check_current_2nd);
			}
		}
	}

	return 0;
}

static struct of_device_id s2mpw01_fuelgauge_match_table[] = {
	{ .compatible = "samsung,s2mpw01-fuelgauge",},
	{},
};
#else
static int s2mpw01_fuelgauge_parse_dt(struct s2mpw01_fuelgauge_data *fuelgauge)
{
	return -ENOSYS;
}

#define s2mpw01_fuelgauge_match_table NULL
#endif /* CONFIG_OF */

static int s2mpw01_fuelgauge_probe(struct platform_device *pdev)
{
	struct s2mpw01_dev *s2mpw01 = dev_get_drvdata(pdev->dev.parent);
	struct s2mpw01_platform_data *pdata = dev_get_platdata(s2mpw01->dev);
	struct s2mpw01_fuelgauge_data *fuelgauge;
	union power_supply_propval raw_soc_val;
	int ret = 0;

	pr_info("%s: S2MPW01 Fuelgauge Driver Loading\n", __func__);

	fuelgauge = kzalloc(sizeof(*fuelgauge), GFP_KERNEL);
	if (!fuelgauge)
		return -ENOMEM;

	mutex_init(&fuelgauge->fg_lock);

	fuelgauge->dev = &pdev->dev;
	fuelgauge->i2c = s2mpw01->fuelgauge;

	fuelgauge->pdata = devm_kzalloc(&pdev->dev, sizeof(*(fuelgauge->pdata)),
			GFP_KERNEL);
	if (!fuelgauge->pdata) {
		dev_err(&pdev->dev, "Failed to allocate memory\n");
		ret = -ENOMEM;
		goto err_parse_dt_nomem;
	}
	ret = s2mpw01_fuelgauge_parse_dt(fuelgauge);
	if (ret < 0)
		goto err_parse_dt;

	platform_set_drvdata(pdev, fuelgauge);

	if (fuelgauge->pdata->fuelgauge_name == NULL)
		fuelgauge->pdata->fuelgauge_name = "sec-fuelgauge";

	fuelgauge->psy_fg.name          = fuelgauge->pdata->fuelgauge_name;
	fuelgauge->psy_fg.type          = POWER_SUPPLY_TYPE_UNKNOWN;
	fuelgauge->psy_fg.get_property  = s2mpw01_fg_get_property;
	fuelgauge->psy_fg.set_property  = s2mpw01_fg_set_property;
	fuelgauge->psy_fg.properties    = s2mpw01_fuelgauge_props;
	fuelgauge->psy_fg.num_properties =
			ARRAY_SIZE(s2mpw01_fuelgauge_props);

	fuelgauge->capacity_max = fuelgauge->pdata->capacity_max;

	s2mpw01_init_regs(fuelgauge);

	if (raw_soc_val.intval == 0)
		raw_soc_val.intval = s2mpw01_get_rawsoc(fuelgauge);

	raw_soc_val.intval = raw_soc_val.intval / 10;

	if (raw_soc_val.intval > fuelgauge->capacity_max)
		s2mpw01_fg_calculate_dynamic_scale(fuelgauge, 100);

	fuelgauge->initial_update_of_soc = true;

	ret = power_supply_register(&pdev->dev, &fuelgauge->psy_fg);
	if (ret) {
		pr_err("%s: Failed to Register psy_fg\n", __func__);
		goto err_data_free;
	}

	if (fuelgauge->pdata->fuel_alert_soc >= 0) {
		s2mpw01_fuelgauge_fuelalert_init(fuelgauge,
					fuelgauge->pdata->fuel_alert_soc);
		wake_lock_init(&fuelgauge->fuel_alert_wake_lock,
					WAKE_LOCK_SUSPEND, "fuel_alerted");

		INIT_DELAYED_WORK(&fuelgauge->isr_work, s2mpw01_fg_isr_work);

		fuelgauge->fg_irq =
			 pdata->irq_base + S2MPW01_FG_IRQ_SOC_L_INT;
		ret = request_threaded_irq(fuelgauge->fg_irq,
				NULL, s2mpw01_fg_irq_thread,
				0, "fuelgauge-irq", fuelgauge);
		if (ret < 0) {
			dev_err(&pdev->dev,
				"%s: Failed to Request IRQ\n", __func__);
			goto err_supply_unreg;
		}

#if 0
		ret = enable_irq_wake(fuelgauge->fg_irq);
		if (ret < 0)
			dev_err(&pdev->dev,
					"%s: Failed to Enable Wakeup Source(%d)\n",
					__func__, ret);

#endif
	}

	pr_info("%s: S2MPW01 Fuelgauge Driver Loaded\n", __func__);
	return 0;

err_supply_unreg:
	power_supply_unregister(&fuelgauge->psy_fg);
err_data_free:
	if (pdev->dev.of_node)
		kfree(fuelgauge->pdata);

err_parse_dt:
err_parse_dt_nomem:
	mutex_destroy(&fuelgauge->fg_lock);
	kfree(fuelgauge);

	return ret;
}

static const struct i2c_device_id s2mpw01_fuelgauge_id[] = {
	{"s2mpw01-fuelgauge", 0},
	{}
};

static void s2mpw01_fuelgauge_shutdown(struct platform_device *pdev)
{
}

static int s2mpw01_fuelgauge_remove(struct platform_device *pdev)
{
	struct s2mpw01_fuelgauge_data *fuelgauge = platform_get_drvdata(pdev);

	if (fuelgauge->pdata->fuel_alert_soc >= 0)
		wake_lock_destroy(&fuelgauge->fuel_alert_wake_lock);

	return 0;
}

#if defined CONFIG_PM
static int s2mpw01_fuelgauge_suspend(struct device *dev)
{
	return 0;
}

static int s2mpw01_fuelgauge_resume(struct device *dev)
{
	return 0;
}
#else
#define s2mpw01_fuelgauge_suspend NULL
#define s2mpw01_fuelgauge_resume NULL
#endif

static SIMPLE_DEV_PM_OPS(s2mpw01_fuelgauge_pm_ops, s2mpw01_fuelgauge_suspend,
		s2mpw01_fuelgauge_resume);

static struct platform_driver s2mpw01_fuelgauge_driver = {
	.driver = {
		.name = "s2mpw01-fuelgauge",
		.owner = THIS_MODULE,
		.pm = &s2mpw01_fuelgauge_pm_ops,
		.of_match_table = s2mpw01_fuelgauge_match_table,
	},
	.probe  = s2mpw01_fuelgauge_probe,
	.remove = s2mpw01_fuelgauge_remove,
	.shutdown   = s2mpw01_fuelgauge_shutdown,
/*	.id_table   = s2mpw01_fuelgauge_id,	*/
};

static int __init s2mpw01_fuelgauge_init(void)
{
	int ret = 0;
	pr_info("%s: S2MPW01 Fuelgauge Init\n", __func__);
	ret = platform_driver_register(&s2mpw01_fuelgauge_driver);

	return ret;
}

static void __exit s2mpw01_fuelgauge_exit(void)
{
	platform_driver_unregister(&s2mpw01_fuelgauge_driver);
}
device_initcall(s2mpw01_fuelgauge_init);
module_exit(s2mpw01_fuelgauge_exit);

MODULE_DESCRIPTION("Samsung S2MPW01 Fuel Gauge Driver");
MODULE_AUTHOR("Samsung Electronics");
MODULE_LICENSE("GPL");
