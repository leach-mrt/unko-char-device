#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h> 

static int major_num=0;
static int minor_num=0;
#define NODE_NAME "unko"
#define SUCCESS 0
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
    raw_copy_to_user(buf, k_buf, len);

    return len;
}

static ssize_t unko_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos){
    printk("unko write\n");
    ssize_t len = 0;
    return ++len;
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
    dev_t dev = MKDEV(major_num, 0);
    alloc_chrdev_region(&dev, 0, minor_num, NODE_NAME);
    major_num = MAJOR(dev);
    cdev_init(&unko_cdev, &unko_fops);
    unko_cdev.owner = THIS_MODULE;
    cdev_add(&unko_cdev, dev, minor_num);

    printk("unko ret SUCCESS\n");
    return SUCCESS;
}

static void unko_exit(void){
    printk("unko exit\n");
    dev_t dev = MKDEV(major_num, 0);
    cdev_del(&unko_cdev);
    unregister_chrdev_region(dev, minor_num);
}

module_init(unko_init);
module_exit(unko_exit);
