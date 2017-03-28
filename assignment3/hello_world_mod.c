#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h> //for module_param
MODULE_LICENSE("GPL");

static char *who = "world";
module_param(who, charp, 0644);
MODULE_PARM_DESC(who, "Tell me your name.");

static int hello_init(void) {
    printk(KERN_ALERT "Hello, %s!\n", who);
    return 0;
}
static void hello_exit(void) {   
    printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
