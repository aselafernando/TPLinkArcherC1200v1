/********************************************************************
 *This is flash ioctl for software upgrade and user config.
 *******************************************************************/


#include <linux/init.h>
#include <linux/fs.h>
#include <linux/major.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <asm/system.h>

#include <typedefs.h>
#include <osl.h>
#include <bcmutils.h>
#include <bcmdevs.h>
#include <bcmnvram.h>
#include <siutils.h>
#include <hndpci.h>
#include <pcicfg.h>
#include <hndsoc.h>
#include <sbchipc.h>
#include <sflash.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <hndsflash.h>
#include <linux/mtd/mtd.h>
/*
 * IOCTL Command Codes
 */
#define BCM_FLASH_READ				0x01
#define BCM_FLASH_WRITE				0x02
#define BCM_FLASH_ERASE				0x03

#define BCM_IO_MAGIC 				0xB3
#define	BCM_IO_FLASH_READ			_IOR(BCM_IO_MAGIC, BCM_FLASH_READ, char)
#define BCM_IO_FLASH_WRITE			_IOW(BCM_IO_MAGIC, BCM_FLASH_WRITE, char)
#define	BCM_IO_FLASH_ERASE			_IO (BCM_IO_MAGIC, BCM_FLASH_ERASE)

#define	BCM_IOC_MAXNR				14
#define flash_major      				239
#define flash_minor      				0

int bcm_flash_ioctl(struct file *file,  unsigned int cmd, unsigned long arg);
int bcm_flash_open (struct inode *inode, struct file *file);

struct file_operations flash_device_op = {
        .owner = THIS_MODULE,
        .unlocked_ioctl = bcm_flash_ioctl,
        .open = bcm_flash_open,
};

static struct cdev flash_device_cdev = {
		//.kobj   = {.name = "bcm_flash_chrdev", },
        .owner  = THIS_MODULE,
		.ops = &flash_device_op,
};

typedef struct 
{
	u_int32_t addr;		/* flash r/w addr	*/
	u_int32_t len;		/* r/w length		*/
	u_int8_t* buf;		/* user-space buffer*/
	u_int32_t buflen;	/* buffer length	*/
	u_int32_t hasHead;	/* hasHead flag 		*/
}ARG;

#define BCM_FLASH_SECTOR_SIZE	(64 * 1024)

struct sflash_info {
	si_t *sih;
	hndsflash_t *sfl;
};
static struct sflash_info flash_info;

#define	BCM_FLASH_LOCK(lock)		if (lock) spin_lock(lock)
#define	BCM_FLASH_UNLOCK(lock)		if (lock) spin_unlock(lock)

extern spinlock_t *partitions_lock_init(void);

/* protect flash i/o */
static spinlock_t *flash_lock;

/* protect flash dev ioctl */
static struct semaphore flash_sem;

static int
bcm_flash_poll(hndsflash_t *sfl, unsigned int offset, int timeout)
{
	int now = jiffies;
	int ret = 0;

	for (;;) {
		if (!hndsflash_poll(sfl, offset))
			break;

		if (time_after((unsigned long)jiffies, (unsigned long)now + timeout)) {
			if (!hndsflash_poll(sfl, offset))
				break;

			printk(KERN_ERR "sflash: timeout\n");
			ret = -ETIMEDOUT;
			break;
		}
		udelay(1);
	}

	return ret;
}

static int
__bcm_flash_read(hndsflash_t *sfl, unsigned int addr, unsigned int len, unsigned char *buf)
{
	
	int bytes;
	while (len > 0)
	{
		BCM_FLASH_LOCK(flash_lock); 
		bytes = hndsflash_read(sfl, addr, len, buf);
		BCM_FLASH_UNLOCK(flash_lock);
		if (bytes  < 0)
		{
			printk("Read Error\n");
			return -1;
		}
		len -= bytes;
		addr += bytes;
		buf += bytes;
	}
	
	return 0;
	
}

static int
__bcm_flash_write(hndsflash_t *sfl, unsigned int addr, unsigned int len, unsigned char *buf)
{
	int ret = 0;
	int bytes = 0;

	BCM_FLASH_LOCK(flash_lock); 

	while(len>0)
	{
		if ((bytes = hndsflash_write(sfl, (uint)addr, len, buf)) < 0)
		{
			ret = bytes;
			printk("hnd write error\n");
			goto  bad;
		}
					
		if (bcm_flash_poll(sfl, (uint)addr, HZ))
		{
			printk("hnd write poll error\n");
			goto bad;
		}

		len -= bytes;
		addr += bytes;
		buf += bytes;
	}


	BCM_FLASH_UNLOCK(flash_lock); 
	printk("#");
	return len;

bad:
	BCM_FLASH_UNLOCK(flash_lock); 
	return ret;
}

static int
__bcm_flash_erase(hndsflash_t *sfl, unsigned int off)
{
	int ret = 0;
	int bytes = 0;

	BCM_FLASH_LOCK(flash_lock); 
	if ((bytes = hndsflash_erase(sfl, off)) < 0) {
		ret = bytes;
		goto bad;
	}

	if ((ret = bcm_flash_poll(sfl, off, 10 * HZ)) < 0)
		goto bad;

	BCM_FLASH_UNLOCK(flash_lock);
	printk("*");
	return bytes;
		
bad:
	BCM_FLASH_UNLOCK(flash_lock); 
	return ret;
}



int nvram_flash_write(hndsflash_t *sfl, u_int8_t *tempData, 
    u_int32_t hasHead, u_int32_t offset, u_int8_t *data, u_int32_t len)
{
	u_int32_t address = 0;
	u_int32_t headLen = 0;
	u_int32_t endAddr = 0, startAddr = 0;
	u_int8_t *orignData = NULL;
	u_int32_t headData[2] = {len, 0};
	u_int32_t frontLen = 0, tailLen = 0;
	u_int32_t retLen = 0;

	headData[0] = htonl(len);	

	if (hasHead != FALSE)
	{
		headLen = 2 * sizeof(u_int32_t);
		len += headLen;
	}

	frontLen = offset % BCM_FLASH_SECTOR_SIZE;
	tailLen  = (offset + len) % BCM_FLASH_SECTOR_SIZE;
	/* 第一个block起始地址 */
	address = offset - frontLen;
	/* 最后一个不完整的block起始地址，如果没有，则是再下一个block起始地址 */
	endAddr = offset + len - tailLen;

	orignData = tempData + BCM_FLASH_SECTOR_SIZE;

	if (frontLen > 0 || headLen > 0)/* 第一个block */
	{
		retLen = __bcm_flash_read(sfl, (uint) address, BCM_FLASH_SECTOR_SIZE, orignData);
		//ath_flash_read(mtd, address, BCM_FLASH_SECTOR_SIZE, &retLen, orignData);
		memcpy(tempData, orignData, frontLen);/* 前面一部分为原来的的数据 */
		
		if (BCM_FLASH_SECTOR_SIZE < frontLen + headLen) /* 头部被拆分到两个blcok */
		{
			headLen = BCM_FLASH_SECTOR_SIZE - frontLen;
			/* 分区头部，第一部分 */
			memcpy(tempData + frontLen, headData, headLen);

			/***************************************************/
			if (memcmp(orignData, tempData, BCM_FLASH_SECTOR_SIZE)) /*内容变化*/
			{
				__bcm_flash_erase(sfl, address);
				retLen = __bcm_flash_write(sfl, (uint)address, BCM_FLASH_SECTOR_SIZE, tempData);
				//ath_flash_sector_erase(address);
				//ath_flash_write(mtd, address, BCM_FLASH_SECTOR_SIZE, &retLen, tempData);
			}
			address += BCM_FLASH_SECTOR_SIZE;
			/***************************************************/
			retLen = __bcm_flash_read(sfl, (uint) address, BCM_FLASH_SECTOR_SIZE, orignData);
			//ath_flash_read(mtd, address, BCM_FLASH_SECTOR_SIZE, &retLen, orignData);
			/* 分区头部，第二部分 */
			memcpy(tempData, (u_int8_t*)(headData) + headLen, 8 - headLen);

			if (len - headLen < BCM_FLASH_SECTOR_SIZE) /*写入数据长度小于一个block*/
			{
				headLen = 8 - headLen;
				copy_from_user(tempData + headLen, data, tailLen - headLen);/* 需要写入的数据 */
				memcpy(tempData + tailLen, orignData + tailLen, BCM_FLASH_SECTOR_SIZE - tailLen);/* 原来的数据 */
				data += tailLen - headLen;
			}
			else
			{
				headLen = 8 - headLen;
				copy_from_user(tempData + headLen, data, BCM_FLASH_SECTOR_SIZE - headLen);/* 需要写入的数据 */
				data += BCM_FLASH_SECTOR_SIZE - headLen;
			}
		}
		else /* 头部未被拆分 */
		{
			memcpy(tempData + frontLen, headData, headLen);/* 分区头部(如果有的话) */
			
			if (len + frontLen < BCM_FLASH_SECTOR_SIZE) /*写入数据长度小于一个block*/
			{
				copy_from_user(tempData + frontLen + headLen, data, len - headLen);/* 后面为需要写入的数据 */
				data += len - headLen;
				/* 再后面是原来的数据 */
				memcpy(tempData + frontLen + len,
						orignData + frontLen + len,
						BCM_FLASH_SECTOR_SIZE - (frontLen + len));
			}
			else
			{
				copy_from_user(tempData + frontLen + headLen, data, BCM_FLASH_SECTOR_SIZE - frontLen - headLen);
				/* 后面为需要写入的数据 */
				data += BCM_FLASH_SECTOR_SIZE - frontLen - headLen;
			}
		}

		/***************************************************/
		if (memcmp(orignData, tempData, BCM_FLASH_SECTOR_SIZE)) /*内容变化*/
		{
			__bcm_flash_erase(sfl, address);
			retLen = __bcm_flash_write(sfl, (uint)address, BCM_FLASH_SECTOR_SIZE, tempData);
		    //ath_flash_sector_erase(address);
		    //ath_flash_write(mtd, address, BCM_FLASH_SECTOR_SIZE, &retLen, tempData);
		}
		address += BCM_FLASH_SECTOR_SIZE;
		/***************************************************/
	}

	if (address < endAddr)/* 中间完整的block，注意:此处用 < 而不是 <=。 */
	{
		startAddr = address;
		while (address < endAddr)
		{
			retLen = __bcm_flash_read(sfl, (uint) address, BCM_FLASH_SECTOR_SIZE, orignData);
			//ath_flash_read(mtd, address, BCM_FLASH_SECTOR_SIZE, &retLen, orignData);
			copy_from_user(tempData, data, BCM_FLASH_SECTOR_SIZE);
			/***************************************************/
			if (memcmp(orignData, tempData, BCM_FLASH_SECTOR_SIZE)) /*内容变化*/
			{
				__bcm_flash_erase(sfl, address);
				retLen = __bcm_flash_write(sfl, (uint)address, BCM_FLASH_SECTOR_SIZE, tempData);
				//ath_flash_sector_erase(address);
				//ath_flash_write(mtd, address, BCM_FLASH_SECTOR_SIZE, &retLen, tempData);
			}
			address += BCM_FLASH_SECTOR_SIZE;
			/***************************************************/
			data += BCM_FLASH_SECTOR_SIZE;
		}
	}

	if (address < offset + len) /* 如果还没有写完，则说明最后有一个不完整的block */
	{
		retLen = __bcm_flash_read(sfl, (uint) address, BCM_FLASH_SECTOR_SIZE, orignData);
		//ath_flash_read(mtd, address, BCM_FLASH_SECTOR_SIZE, &retLen, orignData);
		copy_from_user(tempData, data, tailLen);/*前面一部分为需要写入的数据*/
		memcpy(tempData + tailLen, orignData + tailLen, BCM_FLASH_SECTOR_SIZE - tailLen);
		/*后面为原来数据*/
		/***************************************************/
		if (memcmp(orignData, tempData, BCM_FLASH_SECTOR_SIZE)) /*内容变化*/
		{
			__bcm_flash_erase(sfl, address);
			retLen = __bcm_flash_write(sfl, (uint)address, BCM_FLASH_SECTOR_SIZE, tempData);
		    //ath_flash_sector_erase(address);
		    //ath_flash_write(mtd, address, BCM_FLASH_SECTOR_SIZE, &retLen, tempData);
		}
		address += BCM_FLASH_SECTOR_SIZE;
		/***************************************************/
	}

	return 0;
}


static int 
__bcm_flash_ioctl(struct file *file,  unsigned int cmd, unsigned long arg)
{
	/* temp buffer for r/w */
	unsigned char *rw_buf = (unsigned char *)kmalloc(2 * BCM_FLASH_SECTOR_SIZE, GFP_KERNEL);
	ARG *parg = (ARG*)arg;
	u_int8_t* usr_buf = parg->buf;
	u_int32_t usr_buflen = parg->buflen;
	u_int32_t addr = parg->addr;
	u_int32_t hasHead = parg->hasHead;
	u_int32_t bytes;
	int i;
	int ret = 0;
	int addr_start, addr_end;
	
	int nsector = usr_buflen >> 16; 			/* Divide BCM_FLASH_SECTOR_SIZE */	
	int oddlen = usr_buflen & 0x0000FFFF;	/* odd length (0 ~ BCM_FLASH_SECTOR_SIZE) */
	
	if (rw_buf == NULL)
	{
		printk("rw_buf error\n");
		goto wrong;
	}
	if (_IOC_TYPE(cmd) != BCM_IO_MAGIC)
	{
		printk("cmd type error!\n");
		goto wrong;
	}
	if (_IOC_NR(cmd) > BCM_IOC_MAXNR)
	{
		printk("cmd NR error!\n");
		goto wrong;
	}

	#if 0 /* No need to do this, it would be done in copy_to_user/copy_from_user. by xld, 13May13 */
	if (_IOC_DIR(cmd) & _IOC_READ)
	{
		ret = access_ok(VERIFY_WRITE, (void __user *)usr_buf, _IOC_SIZE(cmd));
	}
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
	{
		ret = access_ok(VERIFY_READ, (void __user *)usr_buf, _IOC_SIZE(cmd));
	}
	#endif

	if (ret < 0)
	{ 
		printk("access no ok!\n");
		goto wrong;
	}
	switch(cmd)
	{
		case BCM_IO_FLASH_READ:
		{
			u_int32_t read_len = usr_buflen;
			u_int32_t bytes = BCM_FLASH_SECTOR_SIZE;

			while (read_len>0)
			{
				if ( bytes >= read_len)
					bytes = read_len;

				if (__bcm_flash_read(flash_info.sfl, (uint) addr, bytes, rw_buf) < 0)
				{
					printk("Read Error\n");
					goto wrong;
				}
				if (copy_to_user(usr_buf, rw_buf,bytes)<0)
				{
					printk("copy to user error\n");
					goto wrong;
				}

				read_len -= bytes;
				addr += bytes;
				usr_buf += bytes;
				
			}
			goto good;
			break;
		}
		case BCM_IO_FLASH_WRITE:
			nvram_flash_write(flash_info.sfl, rw_buf, hasHead, addr, usr_buf, usr_buflen);
			break;
#if 0
		{
			
			addr_start	= addr & 0xFFFF0000;			/* first sector addr to erase */
            addr_end	= (addr + usr_buflen) & 0xFFFF0000;	/* last sector addr to erase */
			
			//local_irq_save(flags);	/* disable interrupts */
			printk("Erase from %#X to %#X:", addr, addr + usr_buflen);
            do
			{
				if ((ret = __bcm_flash_erase(flash_info.sfl, addr_start)) < 0)
					break;
				printk(".");
				addr_start += BCM_FLASH_SECTOR_SIZE;
            }while (addr_start < addr_end);

			printk("\n");
			printk("Program from %#X to %#X:", addr, addr + usr_buflen);
			for (i = 0; i < nsector; i++)		
			{
				char *tmp_buf = rw_buf;
				u_int32_t write_len = BCM_FLASH_SECTOR_SIZE;

				if (copy_from_user(rw_buf, usr_buf, BCM_FLASH_SECTOR_SIZE))
				{
					printk("config write copy_from_usr failed!\n");
					goto wrong;				
				}
				while(write_len)
				{
					if ((bytes = __bcm_flash_write(flash_info.sfl, (uint)addr, write_len, tmp_buf)) < 0)
					{
						goto wrong;
					}
					write_len -= bytes;
					addr += bytes;
					tmp_buf += bytes;
				}
				printk(".");
				usr_buf += BCM_FLASH_SECTOR_SIZE;
			}

			if (oddlen)
			{
				char *tmp_buf = rw_buf;

				if (copy_from_user(rw_buf, usr_buf, oddlen))
				{
	                printk("config write copy_from_usr failed!\n");
	                goto wrong;
	            }
				while (oddlen)
				{
					if ((bytes = __bcm_flash_write(flash_info.sfl, (uint) addr, oddlen, tmp_buf)) < 0)
					{
						goto wrong;
					}
					oddlen -= bytes;
					tmp_buf += bytes;
					addr += bytes;
				}
			}
			printk("\nwrite successfully\n");			
            goto good;
			break;
		}
#endif
		case  BCM_FLASH_ERASE:
		{
			goto good;
			break;
		}
	}

good:
	kfree(rw_buf);
	return 0;
wrong:
	if (rw_buf)
	{
		kfree(rw_buf);
	}

	return -1;
}

int bcm_flash_ioctl(struct file *file,  unsigned int cmd, unsigned long arg)

{
	int retval = 0;

	/* ioctl maybe sleep ! */
	down(&flash_sem);
	retval = __bcm_flash_ioctl(file, cmd, arg);
	up(&flash_sem);

	return retval;
}

int bcm_flash_open (struct inode *inode, struct file *filp)
{
	int minor = iminor(inode);
	int devnum = minor; //>> 1;
	
	if ((filp->f_mode & 2) && (minor & 1)) {
		printk("You can't open the RO devices RW!\n");
		return -EACCES;
	}
	return 0;	
}

#ifdef CONFIG_TPLINK_MEM_ROOTFS
static struct mtd_info *mtdram_info = NULL;

static int mtdram_read(struct mtd_info *mtd, loff_t from, size_t len,
		size_t *retlen, u_char *buf)
{
	if (from + len > mtd->size)
		return -EINVAL;

	memcpy(buf, mtd->priv + from, len);

	*retlen = len;
	return 0;
}

static void mtdram_cleanup(void)
{
    if (mtdram_info) {
        del_mtd_device(mtdram_info);
        kfree(mtdram_info);
        mtdram_info = NULL;
    }
}

static int mtdram_setup(void)
{
    struct mtd_info *mtd;
    extern unsigned int mtd_ram_addr;
    extern unsigned int mtd_ram_size;

    if (mtd_ram_addr == 0 || mtd_ram_size == 0) {
        return 0;
    }

    printk(KERN_NOTICE "############################################################\n");
    printk(KERN_NOTICE "\n%s: booting with mem rootfs@%x/%x.\n\n",
        __func__, mtd_ram_addr, mtd_ram_size);
    printk(KERN_NOTICE "############################################################\n");

    mtd = get_mtd_device_nm("rootfs");
    if (mtd != NULL && mtd != ERR_PTR(-ENODEV)) {
        put_mtd_device(mtd);
        del_mtd_device(mtd);
    } else {
        return -ENODEV;
    }
    
	mtd = kmalloc(sizeof(struct mtd_info), GFP_KERNEL);
	memset(mtd, 0, sizeof(*mtd));

	mtd->name = "rootfs";
	mtd->type = MTD_ROM;
	mtd->flags = MTD_CAP_ROM;
	mtd->size = mtd_ram_size;
	mtd->writesize = 1;
	mtd->priv = (void*)mtd_ram_addr;

	mtd->owner = THIS_MODULE;
	mtd->read = mtdram_read;

    mtdram_info = mtd;
	if (add_mtd_device(mtd)) {
		return -EIO;
	}

	return 0;
}
#endif

int __init bcm_flash_chrdev_init (void)
{
    dev_t dev;
    int ret = 0;
    int err;
    int bcm_flash_major = flash_major;
    int bcm_flash_minor = flash_minor;
	hndsflash_t *info;
#if 0
	struct pci_dev *pci_dev = NULL;
#endif

	/*==================zlw debug==========================*/
	printk(KERN_WARNING "flash_chrdev :  flash_chrdev_init \n");
	/*==================zlw debug==========================*/



    if (bcm_flash_major) {
        dev = MKDEV(bcm_flash_major, bcm_flash_minor);
        ret = register_chrdev_region(dev, 1, "flash_chrdev");
    }
	else {
        ret = alloc_chrdev_region(&dev, bcm_flash_minor, 1, "flash_chrdev");
        bcm_flash_major = MAJOR(dev);
    }

    if (ret < 0) {
        printk(KERN_WARNING "flash_chrdev : can`t get major %d\n", bcm_flash_major);
        goto fail;
    }

    cdev_init (&flash_device_cdev, &flash_device_op);
    err = cdev_add(&flash_device_cdev, dev, 1);
    if (err) 
		printk(KERN_NOTICE "Error %d adding flash_chrdev ", err);
#if 0
	/* Init flash operation reg */
	list_for_each_entry(pci_dev, &((pci_find_bus(0, 0))->devices), bus_list) {
		if ((pci_dev != NULL) && (pci_dev->device == CC_CORE_ID))
			break;
	}

	if (pci_dev == NULL) {
		printk(KERN_ERR "sflash: chipcommon not found\n");
		ret = -ENODEV;
		goto fail;
	}
#endif
	/* attach to the backplane */
	if (!(flash_info.sih = si_kattach(SI_OSH))) {
		printk(KERN_ERR "sflash: error attaching to backplane\n");
		ret = -EIO;
		goto fail;
	}

	/* Initialize serial flash access */
	if (!(info = hndsflash_init(flash_info.sih))) {
		printk(KERN_ERR "bcmsflash: found no supported devices\n");
		ret = -ENODEV;
		goto fail;
	}
	flash_info.sfl = info;

	/* Initialize flash spinlock */
	flash_lock = partitions_lock_init();
	if (!flash_lock) {
		printk(KERN_WARNING "bcmsflash: can't init flash spinlock!\n");
	}

	/* Initialize ioctl semaphore */
	init_MUTEX(&flash_sem);

#ifdef CONFIG_TPLINK_MEM_ROOTFS
	mtdram_setup();
#endif

	return 0;

fail:
	return ret;
}

static void __exit cleanup_bcm_flash_chrdev_exit (void)
{
#ifdef CONFIG_TPLINK_MEM_ROOTFS
	mtdram_cleanup();
#endif
//	unregister_chrdev_region(MKDEV(flash_major, flash_minor), 1);
}


module_init(bcm_flash_chrdev_init);
module_exit(cleanup_bcm_flash_chrdev_exit);
//MODULE_LICENSE("GPL");

