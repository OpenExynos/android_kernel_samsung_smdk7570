/* s6e36w1x01_param.h
 *
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * SeungBeom, Park <sb1.park@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __S6E36W1X01_PARAM_H__
#define __S6E36W1X01_PARAM_H__

static const unsigned char TEST_KEY_ON_0[] = {
	0xF0,
	0x5A, 0x5A,
};

static const unsigned char TEST_KEY_OFF_0[] = {
	0xF0,
	0xA5, 0xA5,
};

static const unsigned char TEST_KEY_ON_1[] = {
	0xF1,
	0x5A, 0x5A,
};

static const unsigned char TEST_KEY_OFF_1[] = {
	0xF1,
	0xA5, 0xA5,
};

static const unsigned char HIDDEN_KEY_ON[] = {
	0xFC,
	0x5A, 0x5A,
};

static const unsigned char HIDDEN_KEY_OFF[] = {
	0xFC,
	0xA5, 0xA5,
};

static const unsigned char SLPIN[] = {
	0x10,
	0x00, 0x00,
};

static const unsigned char SLPOUT[] = {
	0x11,
	0x00, 0x00,
};

static const unsigned char PTLON[] = {
	0x12,
	0x00, 0x00,
};

static const unsigned char NORON[] = {
	0x13,
	0x00, 0x00,
};

static const unsigned char DISPOFF[] = {
	0x28,
	0x00, 0x00,
};

static const unsigned char DISPON[] = {
	0x29,
	0x00, 0x00,
};

static const unsigned char PTLAR[] = {
	0x30,
	0x00, 0x00, 0x01, 0xDF,
};

static const unsigned char TEOFF[] = {
	0x34,
	0x00, 0x00,
};

static const unsigned char TEON[] = {
	0x35,
	0x00, 0x00,
};

static const unsigned char IDMOFF[] = {
	0x38,
	0x00, 0x00,
};

static const unsigned char IDMON[] = {
	0x39,
	0x00, 0x00,
};

static const unsigned char WRCABC_OFF[] = {
	0x55,
	0x00, 0x00,
};

static const unsigned char TEMP_OFFSET_GPARA[] = {
	0xB0,
	0x07, 0x00,
};

static const unsigned char MPS_TEMP_ON[] = {
	0xB6,
	0x8C, 0x00,
};

static const unsigned char MPS_TEMP_OFF[] = {
	0xB6,
	0x88, 0x00,
};

static const unsigned char MPS_TSET_1[] = {
	0xB8,
	0x80, 0x00,
};

static const unsigned char MPS_TSET_2[] = {
	0xB8,
	0x8A, 0x00,
};

static const unsigned char MPS_TSET_3[] = {
	0xB8,
	0x94, 0x00,
};

static const unsigned char DEFAULT_GPARA[] = {
	0xB0,
	0x00, 0x00,
};

static const unsigned char ELVSS_DFLT_GPARA[] = {
	0xB0,
	0x18, 0x00,
};

static const unsigned char MTP1_GPARA[] = {
	0xB0,
	0x05, 0x00,
};

static const unsigned char MTP3_GPARA[] = {
	0xB0,
	0x04, 0x00,
};

static const unsigned char LTPS1_GPARA[] = {
	0xB0,
	0x0F, 0x00,
};

static const unsigned char LTPS2_GPARA[] = {
	0xB0,
	0x54, 0x00,
};

static const unsigned char TEMP_OFFSET[] = {
	0xB6,
	0x00, 0x00, 0x00, 0x05,
	0x05, 0x0C, 0x0C, 0x0C,
	0x0C,
};

static const unsigned char HBM_ELVSS[] = {
	0xB6,
	0x88, 0x11,
};

static const unsigned char HBM_VINT[] = {
	0xF4,
	0x77, 0x0A,
};

static const unsigned char ACL_8P[] = {
	0xB5,
	0x51, 0x99, 0x0A, 0x0A, 0x0A,
};

static const unsigned char HBM_ACL_ON[] = {
	0xB4,
	0x0D, 0x00,
};

static const unsigned char HBM_ACL_OFF[] = {
	0xB4,
	0x09, 0x00,
};

static const unsigned char HBM_ON[] = {
	0x53,
	0xC0, 0x00,
};

static const unsigned char HBM_OFF[] = {
	0x53,
	0x00, 0x00,
};

static const unsigned char ACL_ON[] = {
	0x55,
	0x02, 0x00,
};

static const unsigned char ACL_OFF[] = {
	0x55,
	0x00, 0x00,
};

static const unsigned char SET_ALPM_FRQ[] = {
	0xBB,
	0x00, 0x00, 0x00, 0x00,
	0x01, 0xE0, 0x47, 0x49,
	0x55, 0x00, 0x00, 0x00,
	0x00, 0x0A, 0x0A,
};

static const unsigned char GAMMA_360[] = {
	0xCA,
	0x01, 0x00, 0x01, 0x00,
	0x01, 0x00, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};

static const unsigned char LTPS_TIMING1[] = {
	0xCB,
	0x87, 0x00,
};

static const unsigned char LTPS_TIMING2[] = {
	0xCB,
	0x00, 0x87, 0x69, 0x1A,
	0x69, 0x1A, 0x00, 0x00,
	0x08, 0x03, 0x03, 0x00,
	0x02, 0x02, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F,
};

static const unsigned char IGNORE_EOT[] = {
	0xE7,
	0xEF, 0x67, 0x03, 0xAF,
	0x47,
};

static const unsigned char MDNIE_LITE_CTL1[] = {
	0xEB,
	0x01, 0x00, 0x03, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};

static const unsigned char MDNIE_LITE_CTL2[] = {
	0xEC,
	0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x88, 0x01,
	0x88, 0x01, 0x88, 0x05,
	0x90, 0x05, 0x90, 0x05,
	0x90, 0x05, 0x90, 0x0C,
	0x98, 0x0C, 0x98, 0x0C,
	0x98, 0x0C, 0x98, 0x18,
	0xA0, 0x18, 0xA0, 0x18,
	0xA0, 0x18, 0xA0, 0x18,
	0xA0, 0x48, 0xB5, 0x40,
	0xB2, 0x31, 0xAE, 0x29,
	0x1D, 0x54, 0x16, 0x87,
	0x0F, 0x00, 0xFF, 0x00,
	0xFF, 0xFF, 0x00, 0xFF,
	0x00, 0xFF, 0x00, 0x00,
	0xFF, 0xFF, 0x00, 0xFF,
	0x00, 0xFF, 0x00, 0x00,
	0xFF, 0xFF, 0x00, 0xFF,
	0x00, 0xFF, 0x00,
};

static const unsigned char GRAY_TUNE[] = {
	0xE7,
	0xb3, 0x4c, 0xb3, 0x4c,
	0xb3, 0x4c, 0x69, 0x96,
	0x69, 0x96, 0x69, 0x96,
	0xe2, 0x1d, 0xe2, 0x1d,
	0xe2, 0x1d, 0xff, 0x00,
	0xff, 0x00, 0xff, 0x00,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0xFF,
	0x01, 0x00,
};

static const unsigned char NEGATIVE_TUNE[] = {
	0xE7,
	0xff, 0x00, 0x00, 0xff,
	0x00, 0xff, 0x00, 0xff,
	0xff, 0x00, 0x00, 0xff,
	0x00, 0xff, 0x00, 0xff,
	0xff, 0x00, 0x00, 0xff,
	0x00, 0xff, 0x00, 0xff,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0x20,
	0x00, 0x20, 0x00, 0xFF,
	0x01, 0x00,
};

static const unsigned char OUTDOOR_TUNE[] = {
	0xE7,
	0x00, 0xff, 0xff, 0x00,
	0xff, 0x00, 0xff, 0x00,
	0x00, 0xff, 0xff, 0x00,
	0xff, 0x00, 0xff, 0x00,
	0x00, 0xff, 0xff, 0x00,
	0xff, 0x00, 0xff, 0x00,
	0x00, 0x00, 0x01, 0x88,
	0x01, 0x88, 0x01, 0x88,
	0x05, 0x90, 0x05, 0x90,
	0x05, 0x90, 0x05, 0x90,
	0x0c, 0x98, 0x0c, 0x98,
	0x0c, 0x98, 0x0c, 0x98,
	0x18, 0xa0, 0x18, 0xa0,
	0x18, 0xa0, 0x18, 0xa0,
	0x18, 0xa0, 0x48, 0xb5,
	0x40, 0xb2, 0x31, 0xae,
	0x29, 0x1d, 0x54, 0x16,
	0x87, 0x0f, 0x00, 0xFF,
	0x01, 0x00,
};

static const unsigned char MDNIE_GRAY_ON[] = {
	0xE6,
	0x01, 0x00, 0x30, 0x00,
};

static const unsigned char MDNIE_OUTD_ON[] = {
	0xE6,
	0x01, 0x00, 0x33, 0x05,
};

static const unsigned char MDNIE_CTL_OFF[] = {
	0xE6,
	0x00, 0x00,
};

static const unsigned char SET_DC_VOL[] = {
	0xF5,
	0xC2, 0x03, 0x0B, 0x1B,
	0x7D, 0x57, 0x22, 0x0A,
};

static const unsigned char AOR_360[] = {
	0xB2,
	0x18, 0x00,
};

static const unsigned char ELVSS_360[] = {
	0xB6,
	0x88, 0x1B,
};

static const unsigned char VINT_360[] = {
	0xF4,
	0x77, 0x0A,
};

static const unsigned char PANEL_UPDATE[] = {
	0xF7,
	0x03, 0x00,
};

static const unsigned char ETC_GPARA[] = {
	0xB0,
	0x06, 0x00,
};

static const unsigned char ETC_SET[] = {
	0xFE,
	0x05, 0x00,
};

static const unsigned char AUTO_CLK_ON[] = {
	0xB9,
	0xBE, 0x07, 0x7D, 0x00,
	0x3B, 0x41, 0x00, 0x00,
	0x0A, 0x04, 0x08, 0x00,
};

static const unsigned char AUTO_CLK_OFF[] = {
	0xB9,
	0xA0, 0x07, 0x7D, 0x00,
	0x3B, 0x41, 0x00, 0x00,
	0x0A, 0x04, 0x08, 0x00,
};

static const unsigned char ALPM_ETC[] = {
	0xBB,
	0x90, 0x00,
};

static const unsigned char ALPM_ETC_EXIT[] = {
	0xBB,
	0x91, 0x00,
};

static const unsigned char NORMAL_ON[] = {
	0x53,
	0x00, 0x00,
};

static const unsigned char HLPM_ON[] = {
	0x53,
	0x01, 0x00,
};

static const unsigned char ALPM_ON[] = {
	0x53,
	0x02, 0x00,
};

static const unsigned char ALPM_TEMP_ETC0[] = {
	0xB0,
	0x06, 0x00,
};

static const unsigned char ALPM_TEMP_ETC1[] = {
	0xF6,
	0x00, 0x00,
};

static const unsigned char ALPM_TEMP_ETC2[] = {
	0xB0,
	0x2C, 0x00,
};

static const unsigned char ALPM_TEMP_ETC3[] = {
	0xCB,
	0x5F, 0x4D,
};

static const unsigned char ALPM_TEMP_ETC4[] = {
	0xB0,
	0x60, 0x00,
};

static const unsigned char ALPM_TEMP_ETC5[] = {
	0xCB,
	0x59, 0x2D, 0x00, 0x00,
	0x00, 0x00, 0x06, 0x00,
	0x31, 0x01, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05,
};

static const unsigned char ALPM_TEMP_ETC6[] = {
	0xBB,
	0x91, 0x00, 0x00, 0x80,
	0x50, 0x3D, 0x3B, 0x4D,
	0x3D, 0x3B, 0x4D, 0x00,
	0x18, 0x18,
};

static const unsigned char HLPM_GAMMA_ETC[] = {
	0xCA,
	0x00, 0x68, 0x00, 0x68,
	0x00, 0x52, 0x80, 0x81,
	0x73, 0x77, 0x78, 0x79,
	0x51, 0x51, 0x49, 0x2D,
	0x2B, 0x10, 0x21, 0x2B,
	0x00, 0x21, 0x08, 0x44,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x33, 0x04,
};

static const unsigned char HLPM_ETC[] = {
	0xBB,
	0x10, 0x00,
};

#endif /* __S6E36W1X01_PARAM_H__ */

