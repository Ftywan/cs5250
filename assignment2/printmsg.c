#include <linux/kernel.h> /*for printk */
#include <linux/syscalls.h> /* for SYSCALL_DEFINE4 macro */
#include <linux/slab.h> /* kmalloc */

SYSCALL_DEFINE4(printmsg, 
        char *, name, 
        char *, student_number,
        unsigned long, name_len,
        unsigned long, student_number_len) 
{
    char *name_buff;
    char *student_number_buff;
     
    if ((name_buff = kmalloc(name_len, GFP_KERNEL)) == NULL)
        return -ENOMEM;
    
    if ((student_number_buff = kmalloc(student_number_len, GFP_KERNEL)) == NULL) {
        kfree(name_buff);
        return -ENOMEM;
    }
    
    if (copy_from_user(name_buff, name, name_len)) {
        kfree(name_buff);
        kfree(student_number_buff);
        return -EFAULT;
    }
    
    if (copy_from_user(student_number_buff, student_number, student_number_len)) {
        kfree(name_buff);
        kfree(student_number_buff);
        return -EFAULT;
    }
        
    trace_printk(
        "Hello! My name is %s and my student number is %s. Nice to meet you!\n", 
        name_buff, 
        student_number_buff);
    
    printk(KERN_DEBUG 
            "Hello! My name is %s and my student number is %s. Nice to meet you!", 
            name_buff, 
            student_number_buff);
    
    kfree(name_buff);
    kfree(student_number_buff);
    
    return 0;
}
