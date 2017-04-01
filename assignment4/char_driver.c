#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */ 
//#include <linux/cdev.h>

#define MAJOR_NUMBER 61/* forward declaration */
#define SCULL_IOC_MAGIC  'k' 
#define SCULL_HELLO _IO(SCULL_IOC_MAGIC, 1) 
#define SCULL_W _IOW(SCULL_IOC_MAGIC, 2, char *) 
#define SCULL_R _IOR(SCULL_IOC_MAGIC, 3, char *)
#define SCULL_WR _IOWR(SCULL_IOC_MAGIC, 4, char *) 

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define SCULL_IOC_MAXNR 4
//#define SCULL_QUANTUM 4000
//#define SCULL_QSET    1000
//
//int scull_quantum = SCULL_QUANTUM;
//int scull_qset =    SCULL_QSET;
//
//struct scull_qset {
//    void **data;
//    struct scull_qset *next;
//};
//
//struct scull_dev {
//    struct scull_qset *data;  /* Pointer to first quantum set */
//    int quantum;              /* the current quantum size */
//    int qset;                 /* the current array size */
//    unsigned long size;       /* amount of data stored here */
//    unsigned int access_key;  /* used by sculluid and scullpriv */
//    struct semaphore sem;     /* mutual exclusion semaphore     */
//    struct cdev cdev;	  /* Char device structure		*/
//};
//
//int scull_trim(struct scull_dev *dev)
//{
//    struct scull_qset *next, *dptr;
//    int qset = dev->qset;   /* "dev" is not-null */
//    int i;
//
//    for (dptr = dev->data; dptr; dptr = next) { /* all the list items */
//        if (dptr->data) {
//            for (i = 0; i < qset; i++)
//                kfree(dptr->data[i]);
//            kfree(dptr->data);
//            dptr->data = NULL;
//        }
//        next = dptr->next;
//        kfree(dptr);
//    }
//    dev->size = 0;
//    dev->quantum = scull_quantum;
//    dev->qset = scull_qset;
//    dev->data = NULL;
//    return 0;
//}

int char_open(struct inode *inode, struct file *filep);
int char_release(struct inode *inode, struct file *filep);
long char_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
ssize_t char_read(struct file *filep, 
        char *buf, size_t count, 
        loff_t *f_pos);

ssize_t char_write(struct file *filep, 
        const char *buf, size_t count, 
        loff_t *f_pos);
loff_t char_lseek(struct file *file,
        loff_t offset,
        int orig);

static void char_exit(void);
struct file_operations char_fops = {
    read: char_read,
    write: char_write,
    open: char_open,
    release: char_release,
    llseek: char_lseek,
    unlocked_ioctl: char_ioctl
};

char *char_data = NULL;
const size_t message_size = 1UL << 22;
char dev_msg[30];
//size_t current_size = 1;

int char_open(struct inode *inode, struct file *filep)
{
//    struct scull_dev *dev; /* device information */
//
//    dev = container_of(inode->i_cdev, struct scull_dev, cdev);
//    filep->private_data = dev; /* for other methods */
//
//    /* now trim to 0 the length of the device if open was write-only */
//    if ( (filep->f_flags & O_ACCMODE) == O_WRONLY) {
//        scull_trim(dev); /* ignore errors */
//    }
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
    
    return simple_read_from_buffer(buf, len, f_pos, char_data, message_size); 
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

    return simple_write_to_buffer(char_data, message_size, f_pos, buf, count);
}
loff_t char_lseek(struct file *file,
        loff_t offset,
        int whence)
{
//    struct scull_dev *dev = file->private_data;
    loff_t new_pos;

    switch(whence)
    {
        case SEEK_SET:
            new_pos = offset;
            break;
        case SEEK_CUR:
            new_pos = file->f_pos + offset;
            break;
        case SEEK_END:
//            new_pos = dev->size + offset;
            new_pos = 10 + offset;
            break;
        default: /* can't happen */
            return -EINVAL;
    }          
    
    if( new_pos < 0 ) new_pos = -EINVAL;
    file->f_pos = new_pos;

    return new_pos;
}
long char_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) 
{ 
    int err = 0; 
    ssize_t retval = 0; 
    /* 
      * extract the type and number bitfields, and don't decode 
      * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok() 
      */ 
    if (_IOC_TYPE(cmd) != SCULL_IOC_MAGIC)  return -ENOTTY; 
    if (_IOC_NR(cmd) > SCULL_IOC_MAXNR) return -ENOTTY; 
      /* 
      * the direction is a bitmask, and VERIFY_WRITE catches R/W 
      * transfers. 'Type' is user‐oriented, while 
      * access_ok is kernel‐oriented, so the concept of "read" and 
      * "write" is reversed 
      */ 
    if (_IOC_DIR(cmd) & _IOC_READ) 
        err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd)); 
    else if (_IOC_DIR(cmd) & _IOC_WRITE) 
        err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd)); 
    if (err) return -EFAULT; 
    
    switch(cmd) { 
        case SCULL_HELLO: 
            printk(KERN_WARNING "hello\n"); 
            break; 
        case SCULL_R:
            if (copy_to_user((char *)arg, dev_msg, sizeof(dev_msg))) 
                retval = -EACCES;
            break;
        case SCULL_W:
            if (copy_from_user(dev_msg, (char *)arg, sizeof(dev_msg))) 
                retval = -EACCES; 
            printk(KERN_INFO "user said: '%s'\n", dev_msg);
            break;
        case SCULL_WR:
            retval = copy_from_user(dev_msg, (char *)arg, sizeof(dev_msg));
            printk(KERN_INFO "user said: '%s'\n", dev_msg);
            printk(KERN_INFO "retval = %zu'\n", retval);
            if (retval == 0) {
                retval = copy_to_user((char *)arg, dev_msg, sizeof(dev_msg));
            }
            break;
        default:  /* redundant, as cmd was checked against MAXNR */ 
            retval = -ENOTTY; 
            break;
    } 
    
    return retval; 
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
    char_data = kmalloc(message_size, GFP_KERNEL);
    memset(char_data, 0, message_size);
    
    if (!char_data) {
        char_exit();
        // cannot allocate memory
        // return no memory error, negative signify a failure
        return -ENOMEM;
    }
    // initialize the value to be X
    *char_data = 'X';
    printk(KERN_ALERT "Device module with size %zu loaded.\n", 
        ksize(char_data));
    return 0;
}
static void char_exit(void)
{
    // if the pointer is pointing to something
    if (char_data) {
        // free the memory and assign the pointer to NULL
        kfree(char_data);
        char_data = NULL;
    }
    // unregister the device
    unregister_chrdev(MAJOR_NUMBER, "onebyte");
    printk(KERN_ALERT "Device module is unloaded\n");
}

MODULE_LICENSE("GPL");
module_init(char_init);
module_exit(char_exit);
