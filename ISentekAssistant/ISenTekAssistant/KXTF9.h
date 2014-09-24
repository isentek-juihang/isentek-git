/* include/linux/kxtf9.h - KXTF9 motion sensor driver
 *
 * Copyright (C) 2009 AMIT Technology Inc.
 * Author: 
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
/*
 * Definitions for kxtf9 motion sensor chip.
 */
#ifndef KXTF9_H
#define KXTF9_H

//#include <linux/ioctl.h>

#define KXTF9_I2CSLAVEADD		0x0F

/* KXTF9 Internal Command  (Please refer to KXTF9 Specifications) */
#define KXTF9_REG_XOUT_L			0x06
#define KXTF9_REG_CTRL_REG1			0x1B
#define KXTF9_REG_CTRL_REG2			0x1C
#define KXTF9_REG_CTRL_REG3			0x1D
#define KXTF9_REG_DATA_CTRL			0x21

#define KXTF9_OPERATION				0x80
#define KXTF9_12BIT					0x40
#define KXTF9_DATA_ODR_400HZ		0x05


/* IOCTLs for kxtf9 misc. device library */
#define KXTF9IO						   	  0x87
#define KXTF9_IOCTL_INIT                  _IO(KXTF9IO, 0x01)
#define KXTF9_IOCTL_READ_CHIPINFO         _IOR(KXTF9IO, 0x02, char)
#define KXTF9_IOCTL_READ_SENSORDATA       _IOR(KXTF9IO, 0x03, char)

#define KXTF9_BUFSIZE						256

#endif

