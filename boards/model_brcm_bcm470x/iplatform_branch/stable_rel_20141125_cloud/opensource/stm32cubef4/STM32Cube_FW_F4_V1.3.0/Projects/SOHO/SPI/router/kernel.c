/*
 *  Button Poll driver
 *
 *  author	yaopeiyong
 *
 *  Based on the diag.c - GPIO interface driver for Broadcom boards
 *    Copyright (C) 2006 Mike Baker <mbm@openwrt.org>,
 *    Copyright (C) 2006-2007 Felix Fietkau <nbd@openwrt.org>
 *    Copyright (C) 2008 Andy Boyett <agb@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kmod.h>
#include <linux/init.h>
#include <linux/netfilter_ipv4.h>
#include <linux/spi/spi.h>


#define DRV_NAME	"spi"
#define DRV_VERSION	"0.0.1"
#define DRV_DESC	"spi driver"




#define SPI_BUF_SIZE	(8 * 1024)

#define BASE_CTL       2048

#define SET_DATA		BASE_CTL
#define SET_MAX			SET_DATA

#define GET_DATA		BASE_CTL
#define GET_MAX			GET_DATA

static struct spi_master *mast;
struct spi_device *ssdspi;



EXPORT_SYMBOL_GPL(ssdspi);
char * nvram_get(const char *name);
long simple_strtol(const char *cp, char **endp, unsigned int base);

static int do_spi_set(struct sock *sk, int cmd, void __user *user, unsigned int len)
{
	if (cmd == SET_DATA)
	{
		unsigned char *wbuf = (unsigned char *)kmalloc(SPI_BUF_SIZE,GFP_KERNEL);
		if (wbuf == NULL)
		{
			printk("wbuf error\n");
			goto wrong;
		}

		if (len > SPI_BUF_SIZE)
		{
			printk("len is too large\n");
			goto wrong;
		}

		copy_from_user(wbuf,user,len);

		spi_write(ssdspi,wbuf,len);

		kfree(wbuf);
		return 0;
		wrong:

		if (wbuf)
		{
			kfree(wbuf);
		}
		return -1;

	}
	return 0;
}

static int do_spi_get(struct sock *sk, int cmd, void __user *user, int* len)
{
	if (cmd == GET_DATA)
	{
		unsigned char *rbuf = (unsigned char *)kmalloc(SPI_BUF_SIZE,GFP_KERNEL);
		if (rbuf == NULL)
		{
			printk("rbuf error\n");
			goto wrong;
		}

		if (*len > SPI_BUF_SIZE)
		{
			printk("len is too large\n");
			goto wrong;
		}

		spi_read(ssdspi,rbuf,*len);

		copy_to_user(user,rbuf,*len);

		kfree(rbuf);
		return 0;
		wrong:

		if (rbuf)
		{
			kfree(rbuf);
		}
		return -1;
	}

	return 0;
}

static struct nf_sockopt_ops spi_sockopts = {
	.pf			= PF_INET,
	.set_optmin	= BASE_CTL,
	.set_optmax	= SET_MAX+1,
	.set		= do_spi_set,
	.get_optmin	= BASE_CTL,
	.get_optmax	= GET_MAX+1,
	.get		= do_spi_get,
};


char buf[12];

static int __init spi_lib_init(void)
{
	
	dev_t dev;
	int ret = 0;
	char *speed;

	mast = spi_busnum_to_master(1);
	
	ssdspi = spi_alloc_device(mast);

	speed = nvram_get("spi_speed");
	if (speed != NULL)
	{
		ssdspi->max_speed_hz= simple_strtol(speed, NULL, 10);
	}
	else
	{
		ssdspi->max_speed_hz= 20000000;
	}
	
	if (spi_add_device(ssdspi) <0)
	{
		printk("eror\n");
	}

	struct spi_device *spi = ssdspi;


	printk( "setup mode %d, %s%s%s%s"
				"%u bits/w, %u Hz max ",
			(int) (spi->mode & (SPI_CPOL | SPI_CPHA)),
			(spi->mode & SPI_CS_HIGH) ? "cs_high, " : "",
			(spi->mode & SPI_LSB_FIRST) ? "lsb, " : "",
			(spi->mode & SPI_3WIRE) ? "3wire, " : "",
			(spi->mode & SPI_LOOP) ? "loopback, " : "",
			spi->bits_per_word, spi->max_speed_hz);


	ret = nf_register_sockopt(&spi_sockopts);
	if (ret < 0) {
		printk("Unable to register sockopts.\n");
		return ret;
	}

	spi_read(ssdspi,buf,12);

	int i;
    for(i=0;i<12;i++)
    {
    	printk("%d ",buf[i]);
    }
    printk("\n");

	
	return ret;
}

module_init(spi_lib_init);

static void __exit spi_lib_exit(void)
{
	spi_unregister_device(ssdspi);
	nf_unregister_sockopt(&spi_sockopts);
}

module_exit(spi_lib_exit);

MODULE_DESCRIPTION(DRV_DESC);
MODULE_VERSION(DRV_VERSION);
MODULE_AUTHOR("yaopeiyong");
MODULE_LICENSE("GPL v2");

