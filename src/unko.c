#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h> // copy_from_user

#define MAJOR_NUM 0
#define NODE_NAME "unko"
#define SUCCESS 0

MODULE_LICENSE("MIT");


static int unko_open(struct inode *inode, struct file *file) {
    return SUCCESS;
}

static ssize_t unko_read(struct file *file, char __user *buf, size_t count, loff_t *f_pos) {
    return 0;
}

static ssize_t unko_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos){
    return 0;
}

static int unko_release(struct inode *inode, struct file *file) {
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
    register_chrdev(MAJOR_NUM, NODE_NAME, &unko_fops);
    return SUCCESS;
}

static void unko_exit(void){
        unregister_chrdev(MAJOR_NUM, NODE_NAME);
}

module_init(unko_init);
module_exit(unko_exit);
