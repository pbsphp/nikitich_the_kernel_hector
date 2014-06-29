#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#include "abuse_generator.h"


#define DEVICE_NAME "nikitich"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pbs <pbsphp@yandex.ru>");
MODULE_DESCRIPTION("Nikitich - the kernel hector");
MODULE_SUPPORTED_DEVICE(DEVICE_NAME);

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static int major_number;
static int is_device_open = 0;
static char phrase[MAX_PHRASE_LENGTH];
static char *phrase_ptr = 0;



static struct file_operations fops =
{
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};



int init_module(void)
{
    printk(KERN_INFO "Nikitich is coming!\n");

    major_number = register_chrdev(0, DEVICE_NAME, &fops);

    if (major_number < 0) {
        printk(KERN_ERR "It is not Nikitich.\n");
        return major_number;
    }

    printk(KERN_INFO "Please, create a dev file with 'mknod /dev/" \
            DEVICE_NAME " c %d 0'.\n", major_number);

    return 0;
}



void cleanup_module(void)
{
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Nikitich gone away.\n");
}



static int device_open(struct inode *inode, struct file *file)
{
    if (is_device_open) {
        return -EBUSY;
    }

    ++is_device_open;

    get_random_phrase(phrase);
    phrase_ptr = phrase;

    return 0;
}



static int device_release(struct inode *inode, struct file *file)
{
    --is_device_open;
    return 0;
}



static ssize_t device_write(struct file *filp, const char *buff,
                            size_t len, loff_t *off)
{
    return -EINVAL;
}



static ssize_t device_read(struct file *filp, char *buffer,
                            size_t length, loff_t *offset)
{
    int read_bytes = 0;

    while (length != 0 && *phrase_ptr != '\0') {
        put_user(*phrase_ptr, buffer);
        ++phrase_ptr;
        ++buffer;
        --length;
        ++read_bytes;
    }

    return read_bytes;
}
