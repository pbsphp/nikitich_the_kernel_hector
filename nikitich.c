#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>




int init_module(void)
{
    printk(KERN_INFO "Nikitich is coming!\n");

    return 0;
}



void cleanup_module(void)
{
    printk(KERN_INFO "Nikitich gone away.\n");
}
