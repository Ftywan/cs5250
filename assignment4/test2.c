#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <sys/ioctl.h>  
#include <unistd.h>
#include <string.h> 

#define SCULL_IOC_MAGIC 'k' 
#define SCULL_HELLO _IO(SCULL_IOC_MAGIC, 1) 
#define SCULL_W _IOW(SCULL_IOC_MAGIC, 2, char *) 
#define SCULL_R _IOR(SCULL_IOC_MAGIC, 3, char *)
#define SCULL_WR _IOWR(SCULL_IOC_MAGIC, 4, char *) 

#define READ_AND_WRITE 0
#define READ_WRITE 1

int char_driver; 

void test() 
{ 
    int k; 
    char s[3]; 

    memset(s, '2', sizeof(s)); 
    printf("test begin!\n"); 

    k = write(char_driver, s, sizeof(s)); 
    printf("written = %d\n", k); 
    
    k = ioctl(char_driver, SCULL_HELLO); 
    if (k == -1) perror("hello:");
    printf("result = %d\n", k);  
} 

void read_and_write() {
    int k; 
    const char sample[] = "hello kernel!";
    char user_msg[30];
    
    memset(user_msg, '\0', 30);
    
    printf("\n## setting value \"%s\" in kernel. ##\n", sample);
    k = ioctl(char_driver, SCULL_W, sample); 
    if (k == -1) perror("write:");
    printf("result = %d\n", k); 
    
    printf("\n## user_msg value before ioctl call: \"%s\" ##\n", user_msg);
    k = ioctl(char_driver, SCULL_R, user_msg);
    if (k == -1) perror("read:");
    printf("user_msg value after ioctl call: \"%s\"\n", user_msg); 
}

void read_write() {
    int k; 
    const char sample[] = "hello kernel using rw!";
    char user_msg[30];
    
    memset(user_msg, '\0', 30);
    printf("\n## setting dev_msg value using ioctl_rw ##\n");
    k = ioctl(char_driver, SCULL_WR, sample);
    if (k == -1) perror("write:");
    printf("result = %d\n", k); 
    
    printf("\n## getting value of dev_msg and saving it in user_msg thru rw ##\n");
    printf("kernel said: \"%s\"\n", sample);
    k = ioctl(char_driver, SCULL_WR, user_msg);
    if (k == -1) perror("read:");
    printf("result = %d\n", k); 
}

int main(int argc, char **argv) 
{ 
    int rc = 0;
    char_driver = open("/dev/char_driver", O_RDWR); 
    if (char_driver == -1) { 
        perror("unable to open lcd"); 
        exit(EXIT_FAILURE); 
    }
    
    if (argc == 1) {
        printf("Allowed values:\n");
        printf("\t --test to run test only\n");
        printf("\t --read_and_write to run _IOR and _IOW only\n");
        printf("\t --read_write to run _IOWR only\n");
        printf("\t --all to run all tests\n");
    } else if (!strcmp(argv[1], "--test")) test();
    else if (!strcmp(argv[1], "--read_and_write")) read_and_write();
    else if (!strcmp(argv[1], "--read_write")) read_write();
    else if (!strcmp(argv[1], "--all")) {
        test();
        read_and_write();
        read_write();
    } else {
        printf("invalid command \"%s\" passed. exiting now.\n", argv[1]);
        rc = -1;
    }

    close(char_driver); 
    return rc; 
} 