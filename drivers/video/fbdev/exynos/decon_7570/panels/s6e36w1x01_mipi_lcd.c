/* s6e36w1x01_mipi_lcd.c
 *
 * Samsung SoC MIPI LCD driver.
 *
 * Copyright (c) 2015 Samsung Electronics
 *
 * SeungBeom, Park <sb1.parki@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/delay.h>
#include <linux/gpio.h>
#include <video/mipi_display.h>
#include <linux/platform_device.h>

#include "../dsim.h"
#include "lcd_ctrl.h"
#include "decon_lcd.h"
#include "s6e36w1x01_param.h"

#define MAX_BRIGHTNESS 256
#define MIN_BRIGHTNESS 0
#define DEFAULT_BRIGHTNESS 80

static struct dsim_device *dsim_base;
struct backlight_device *bd;
#ifdef CONFIG_HAS_EARLYSUSPEND
static struct early_suspend    s6e36w1x01_early_suspend;
#endif

static int s6e36w1x01_get_brightness(struct backlight_device *bd)
{
	return bd->props.brightness;
}

static int get_backlight_level(int brightness)
{
	int backlightlevel;

	switch (brightness) {
	case 0:
		backlightlevel = 0;
		break;
	case 1 ... 29:
		backlightlevel = 0;
		break;
	case 30 ... 34:
		backlightlevel = 1;
		break;
	case 35 ... 39:
		backlightlevel = 2;
		break;
	case 40 ... 44:
		backlightlevel = 3;
		break;
	case 45 ... 49:
		backlightlevel = 4;
		break;
	case 50 ... 54:
		backlightlevel = 5;
		break;
	case 55 ... 64:
		backlightlevel = 6;
		break;
	case 65 ... 74:
		backlightlevel = 7;
		break;
	case 75 ... 83:
		backlightlevel = 8;
		break;
	case 84 ... 93:
		backlightlevel = 9;
		break;
	case 94 ... 103:
		backlightlevel = 10;
		break;
	case 104 ... 113:
		backlightlevel = 11;
		break;
	case 114 ... 122:
		backlightlevel = 12;
		break;
	case 123 ... 132:
		backlightlevel = 13;
		break;
	case 133 ... 142:
		backlightlevel = 14;
		break;
	case 143 ... 152:
		backlightlevel = 15;
		break;
	case 153 ... 162:
		backlightlevel = 16;
		break;
	case 163 ... 171:
		backlightlevel = 17;
		break;
	case 172 ... 181:
		backlightlevel = 18;
		break;
	case 182 ... 191:
		backlightlevel = 19;
		break;
	case 192 ... 201:
		backlightlevel = 20;
		break;
	case 202 ... 210:
		backlightlevel = 21;
		break;
	case 211 ... 220:
		backlightlevel = 22;
		break;
	case 221 ... 230:
		backlightlevel = 23;
		break;
	case 231 ... 240:
		backlightlevel = 24;
		break;
	case 241 ... 250:
		backlightlevel = 25;
		break;
	case 251 ... 255:
		backlightlevel = 26;
		break;
	default:
		backlightlevel = 12;
		break;
	}

	return backlightlevel;
}

static int update_brightness(int brightness)
{
	int backlightlevel;

	backlightlevel = get_backlight_level(brightness);
	return 1;
}

static int s6e36w1x01_set_brightness(struct backlight_device *bd)
{
	int brightness = bd->props.brightness;

	if (brightness < MIN_BRIGHTNESS || brightness > MAX_BRIGHTNESS) {
		printk(KERN_ALERT "Brightness should be in the range of 0 ~ 255\n");
		return -EINVAL;
	}

	update_brightness(brightness);

	return 1;
}

static const struct backlight_ops s6e36w1x01_backlight_ops = {
	.get_brightness = s6e36w1x01_get_brightness,
	.update_status = s6e36w1x01_set_brightness,
};

static int s6e36w1x01_probe(struct dsim_device *dsim)
{
	dsim_base = dsim;

	bd = backlight_device_register("pwm-backlight.0", NULL,
		NULL, &s6e36w1x01_backlight_ops, NULL);
	if (IS_ERR(bd))
		printk(KERN_ALERT "failed to register backlight device!\n");

	bd->props.max_brightness = MAX_BRIGHTNESS;
	bd->props.brightness = DEFAULT_BRIGHTNESS;

	return 1;
}

static int s6e36w1x01_displayon(struct dsim_device *dsim)
{
	lcd_init(&dsim->lcd_info);
	return 1;
}

static int s6e36w1x01_suspend(struct dsim_device *dsim)
{
	lcd_disable();
	return 1;
}

static int s6e36w1x01_resume(struct dsim_device *dsim)
{
	return 1;
}

struct mipi_dsim_lcd_driver s6e36w1x01_mipi_lcd_driver = {
	.probe		= s6e36w1x01_probe,
	.displayon	= s6e36w1x01_displayon,
	.suspend	= s6e36w1x01_suspend,
	.resume		= s6e36w1x01_resume,
};
