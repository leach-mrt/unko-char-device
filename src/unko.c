#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h> 

static int minor_base=0;
static int num_minor=1;
#define NODE_NAME "unko"
#define SUCCESS 0

static unsigned int major_num;

static struct cdev unko_cdev;

MODULE_LICENSE("MIT");

static unsigned char k_buf[] = "unko!";


static int unko_open(struct inode *inode, struct file *file) {
    printk("unko open\n");
    return SUCCESS;
}

static ssize_t unko_read(struct file *file, char __user *buf, size_t count, loff_t *f_pos) {
    printk("unko read\n");
    if (count <= 0) {
        return 0;
    }

    ssize_t len = strlen(k_buf);

    if(*f_pos >= len) {
        return 0;
    }

    if(raw_copy_to_user(buf, k_buf, len)) {
        return -EFAULT;
    }
    *f_pos += len;

    return len;
}

static ssize_t unko_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos){
    printk("unko write\n");
    ssize_t len = 0;
    return count;
}

static int unko_release(struct inode *inode, struct file *file) {
    printk("unko release\n");
    return SUCCESS;
}

struct file_operations unko_fops = {
    .owner = THIS_MODULE,
    .read = unko_read,
    .write = unko_write,
    .open = unko_open,
    .release = unko_release,
};

static int unko_init(void) {
    printk("unko init\n");

    int alloc_ret = 0;
    int cdev_err = 0;
    dev_t dev;

    // allocate major number
    alloc_ret = alloc_chrdev_region(&dev, major_num, num_minor, NODE_NAME);
    if (alloc_ret != 0) {
        printk(KERN_ERR "alloc_chdev_region = %d\n", alloc_ret);
        return -1;
    }

    // get major number by using dev
    major_num = MAJOR(dev);
    dev = MKDEV(major_num, minor_base);

    // init cdev struct and registry it to system call handler table
    cdev_init(&unko_cdev, &unko_fops);
    unko_cdev.owner = THIS_MODULE;

    // add this driver to kernel
    cdev_err = cdev_add(&unko_cdev, dev, num_minor);
    if (cdev_err != 0) {
        printk(KERN_ERR "cdev_add = %d\n", alloc_ret);
        unregister_chrdev_region(dev, num_minor);
        return -1;
    }

    printk("unko ret SUCCESS\n");
    return SUCCESS;
}

static void unko_exit(void){
    printk("unko exit\n");

    dev_t dev = MKDEV(major_num, minor_base);

    // remove this device driver from kernel
    cdev_del(&unko_cdev);

    // remove major number which this driver used.
    unregister_chrdev_region(dev, num_minor);
}

module_init(unko_init);
module_exit(unko_exit);
