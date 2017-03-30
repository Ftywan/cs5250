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

int char_open(struct inode *inode, struct file *filep);
int char_release(struct inode *inode, struct file *filep);
ssize_t char_read(struct file *filep, 
        char *buf, size_t count, 
        loff_t *f_pos);

ssize_t char_write(struct file *filep, 
        const char *buf, size_t count, 
        loff_t *f_pos);

static void char_exit(void);
struct file_operations char_fops = {
    read: char_read,
    write: char_write,
    open: char_open,
    release: char_release
};
char *onebyte_data = NULL;
const ssize_t message_size = 1UL << 22;
int char_open(struct inode *inode, struct file *filep)
{
    return 0; // always successful
}
int char_release(struct inode *inode, struct file *filep)
{
    return 0; // always successful
}
ssize_t char_read(struct file *filep, 
        char *buf, 
        size_t len, 
        loff_t *f_pos)
{
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
ssize_t char_write(struct file *filep, 
        const char *buf,
        size_t count, 
        loff_t *f_pos)
{   
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
    
    return simple_write_to_buffer(onebyte_data, message_size, f_pos, buf, count);
}
static int char_init(void)
{
    int result;
    // register the device
    result = register_chrdev(MAJOR_NUMBER, "onebyte", &char_fops);
    if (result < 0) {
        return result;
    }
    // allocate one byte of memory for storage
    // kmalloc is just like malloc, the second parameter is
    // the type of memory to be allocated.
    // To release the memory allocated by kmalloc, use kfree.
    onebyte_data = kmalloc(message_size, GFP_KERNEL);
    memset(onebyte_data, 0, message_size);
    
    if (!onebyte_data) {
        char_exit();
        // cannot allocate memory
        // return no memory error, negative signify a failure
        return -ENOMEM;
    }
    // initialize the value to be X
    *onebyte_data = 'X';
    printk(KERN_ALERT "Device module with size %zu loaded.\n", 
        ksize(onebyte_data));
    return 0;
}
static void char_exit(void)
{
    // if the pointer is pointing to something
    if (onebyte_data) {
        // free the memory and assign the pointer to NULL
        kfree(onebyte_data);
        onebyte_data = NULL;
    }
    // unregister the device
    unregister_chrdev(MAJOR_NUMBER, "onebyte");
    printk(KERN_ALERT "Device module is unloaded\n");
}
MODULE_LICENSE("GPL");
module_init(char_init);
module_exit(char_exit);
