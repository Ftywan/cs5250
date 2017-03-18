#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#define MAJOR_NUMBER 61/* forward declaration */

int onebyte_open(struct inode *inode, struct file *filep);
int onebyte_release(struct inode *inode, struct file *filep);
ssize_t onebyte_read(struct file *filep, 
        char *buf, size_t count, 
        loff_t *f_pos);

ssize_t onebyte_write(struct file *filep, 
        const char *buf, size_t count, 
        loff_t *f_pos);

static void onebyte_exit(void);
/* definition of file_operation structure */
struct file_operations onebyte_fops = {
    read: onebyte_read,
    write: onebyte_write,
    open: onebyte_open,
    release: onebyte_release
};
char *onebyte_data = NULL;
const ssize_t message_size = 1;
int onebyte_open(struct inode *inode, struct file *filep)
{
    return 0; // always successful
}
int onebyte_release(struct inode *inode, struct file *filep)
{
    return 0; // always successful
}
ssize_t onebyte_read(struct file *filep, 
        char *buf, 
        size_t len, 
        loff_t *f_pos)
{
    /*please complete the function on your own*/
    /*use helper function from libfs to properly copy data from 
     kernel space to user space
     ssize_t simple_read_from_buffer(void __user *to, size_t count, loff_t *ppos,
                                            const void *from, size_t available)
     * 
     * @to: the user space buffer to read to
     * @count: the maximum number of bytes to read
     * @ppos: the current position in the buffer
     * @from: the buffer to read from
     * @available: the size of the buffer
     */
    
    return simple_read_from_buffer(buf, len, f_pos, onebyte_data, message_size); 
}
ssize_t onebyte_write(struct file *filep, 
        const char *buf,
        size_t count, 
        loff_t *f_pos)
{
    /*please complete the function on your own*/
    ssize_t size;
    ssize_t rc = message_size;
    
    /*use helper function from libfs to safely copy user space data to 
     kernel space
     ssize_t simple_write_to_buffer(void *to, size_t available, loff_t *ppos,
                                    const void __user *from, size_t count)
     * @to: the buffer to write to
     * @available: the size of the buffer
     * @ppos: the current position in the buffer
     * @from: the user space buffer to read from
     * @count: the maximum number of bytes to read
     */
    
    size = simple_write_to_buffer(onebyte_data, message_size, f_pos, buf , count);
    if (count > 1) rc = -ENOSPC;
    return rc;
}
static int onebyte_init(void)
{
    int result;
    // register the device
    result = register_chrdev(MAJOR_NUMBER, "onebyte", &onebyte_fops);
    if (result < 0) {
        return result;
    }
    // allocate one byte of memory for storage
    // kmalloc is just like malloc, the second parameter is// the type of memory to be allocated.
    // To release the memory allocated by kmalloc, use kfree.
    onebyte_data = kmalloc(sizeof(char), GFP_KERNEL);
    if (!onebyte_data) {
        onebyte_exit();
        // cannot allocate memory
        // return no memory error, negative signify a failure
        return -ENOMEM;
    }
    // initialize the value to be X
    *onebyte_data = 'X';
    printk(KERN_ALERT "This is a onebyte device module.\n");
    return 0;
}
static void onebyte_exit(void)
{
    // if the pointer is pointing to something
    if (onebyte_data) {
        // free the memory and assign the pointer to NULL
        kfree(onebyte_data);
        onebyte_data = NULL;
    }
    // unregister the device
    unregister_chrdev(MAJOR_NUMBER, "onebyte");
    printk(KERN_ALERT "Onebyte device module is unloaded\n");
}
MODULE_LICENSE("GPL");
module_init(onebyte_init);
module_exit(onebyte_exit);