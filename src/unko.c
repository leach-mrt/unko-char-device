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
static const char *unko_mp3 = "/usr/share/sounds/unko/unko.mp3";
static unsigned int major_num;
static struct cdev unko_cdev;
static unsigned char k_buf[] = "unko!";
MODULE_LICENSE("GPL");

static int unko_open(struct inode *inode, struct file *file) {
    printk("unko open\n");
    return SUCCESS;
}

struct file *file_open(const char *path, int flags, int rights, int *err) 
{
    struct file *filp = NULL;
    mm_segment_t oldfs;
    *err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    printk("filp open\n");
    filp = filp_open(path, flags, rights);
    printk("filp open end\n");
    set_fs(oldfs);
    if (IS_ERR(filp)) {
        printk("filp open error\n");
        *err = PTR_ERR(filp);
        return NULL;
    }
    return filp;
}

void file_close(struct file *file) 
{
    filp_close(file, NULL);
}

int file_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) 
{
    printk("file_read start\n");
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    printk("start vfs_read\n");
    ret = vfs_read(file, data, size, &offset);
    printk("end vfs_read\n");

    set_fs(oldfs);
    return ret;
}   

int play_unko(void) {
    int err = 0;
    struct file *f = file_open(unko_mp3, O_RDONLY, 0, &err);
    printk("file_open end\n");
    if (f == NULL) {
        printk("can't open mp3 file. reason; err:%d\n", err);
        return 0;
    }

    char data[1024000]; // read buffer
    printk("data buffer\n");
    int ret = file_read(f, 0, &data[0], sizeof(data));
    if (ret == 0) {
        goto close;
    }
    if (ret < 0) {
        printk("can't read file; err:%d\n", ret);
        goto close;
    }

close:
    printk("close. ret:%d\n", ret);
    file_close(f);
    return ret;
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

    play_unko();

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
