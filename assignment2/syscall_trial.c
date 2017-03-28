#include <linux/unistd.h>
#define __NR_printmsg 329

int printmsg(char *, char *, unsigned long, unsigned long);

int main() {
    char name[] = "John Vincent";
    char student_number[] = "A0138204N";
    int rc = -1;
    
    unsigned long name_len = sizeof(name) - 1;
    unsigned long student_number_len = sizeof(student_number) - 1;
    
    rc = printmsg(name, student_number, name_len, student_number_len);
    return rc;
}

int printmsg(char *name, 
        char *student_number,
        unsigned long name_len,
        unsigned long student_number_len)
{   
    return syscall(__NR_printmsg, 
            name, 
            student_number, 
            name_len, 
            student_number_len);
}
