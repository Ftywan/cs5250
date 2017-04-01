#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <sys/ioctl.h>  
#include <unistd.h>
#include <string.h> 

#define SCULL_IOC_MAGIC 'k' 
#define SCULL_HELLO _IO(SCULL_IOC_MAGIC, 1) 
#define SCULL_WRITE _IOW(SCULL_IOC_MAGIC, 2, char *) 
#define SCULL_READ _IOR(SCULL_IOC_MAGIC, 3, char *) 

int char_driver; 
const char sample[] = "hello kernel!";

void test() 
{ 
    int k; 
    char s[3]; 
    char user_msg[30];

    memset(s, '2', sizeof(s)); 
    memset(user_msg, 'x', 30);
    printf("test begin!\n"); 

    k = write(char_driver, s, sizeof(s)); 
    printf("written = %d\n", k); 
    
    k = ioctl(char_driver, SCULL_HELLO); 
    if (k == -1) perror("hello:");
    printf("result = %d\n", k); 
    
    printf("setting value '%s' in kernel.\n", sample);
    k = ioctl(char_driver, SCULL_WRITE, sample); 
    if (k == -1) perror("write:");
    printf("result = %d\n", k); 
    
    printf("user_msg value before ioctl call: %s\n", user_msg);
    k = ioctl(char_driver, SCULL_READ, user_msg);
    if (k == -1) perror("read:");
    printf("user_msg value after ioctl call: %s\n", user_msg); 
     
} 
int main(int argc, char **argv) 
{ 
    char_driver = open("/dev/char_driver", O_RDWR); 
    if (char_driver == -1) { 
        perror("unable to open lcd"); 
        exit(EXIT_FAILURE); 
    }
 
    test(); 
    close(char_driver); 
    return 0; 
} 