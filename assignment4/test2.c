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
void test();
void read_and_write();
void read_write();

int main(int argc, char **argv) 
{ 
    int rc = 0;
    char_driver = open("/dev/char_driver", O_RDWR); 
    if (char_driver == -1) { 
        perror("unable to open lcd"); 
        exit(EXIT_FAILURE); 
    }
    
    printf("\n");
    if (argc == 1 || argc > 2) {
        printf("Simple testbench to test ioctl functions.\n");
        printf("_IOR, _IOW and _IOWR are implemented in this testbench.\n");
        printf("Only one parameter can be passed at a time.\n");
        printf("\nAllowed values:\n");
        printf("\t-T, --test\n");
        printf("\t\tto run initial test only\n");
        printf("\t-RAW, --read_and_write\n");
        printf("\t\tto run _IOR and _IOW only\n");
        printf("\t--RW, --read_write\n");
        printf("\t\tto run  _IOWR only\n");
        printf("\t-A, --all\n");
        printf("\t\tto run all tests\n");
    } else if (!strcmp(argv[1], "--test") || !strcmp(argv[1], "-T")) 
        test();
    else if (!strcmp(argv[1], "--read_and_write") || !strcmp(argv[1], "-RAW")) 
        read_and_write();
    else if (!strcmp(argv[1], "--read_write") || !strcmp(argv[1], "-RW")) 
        read_write();
    else if (!strcmp(argv[1], "--all") || !strcmp(argv[1], "-A")) {
        printf("INIT Test:\n");
        test();
        printf("\nREAD_AND_WRITE Test:\n");
        read_and_write();
        printf("\nREAD_WRITE Test:\n");
        read_write();
    } else {
        printf("invalid command \"%s\" passed. exiting now.\n", argv[1]);
        rc = -1;
    }

    printf("\n");
    close(char_driver); 
    return rc; 
} 

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
    printf("ioctl result = %d\n", k);  
} 

void read_and_write() {
    int k; 
    const char sample[] = "hello kernel!";
    char user_msg[30];
    
    memset(user_msg, '\0', 30);
    
    printf("\nsetting value \"%s\" in kernel.\n", sample);
    k = ioctl(char_driver, SCULL_W, sample); 
    if (k == -1) perror("write:");
    printf("ioctl result = %d\n", k); 
    
    printf("\nuser_msg value before ioctl call: \"%s\" \n", user_msg);
    k = ioctl(char_driver, SCULL_R, user_msg);
    if (k == -1) perror("read:");
    printf("user_msg value after ioctl call: \"%s\"\n", user_msg); 
    printf("ioctl result = %d\n", k); 
}

void read_write() {
    int k; 
    const char sample[] = "hello kernel using rw!";
    char user_msg[30];
    
    memset(user_msg, '\0', 30);
    printf("\nsetting dev_msg value using _IOWR call\n");
    k = ioctl(char_driver, SCULL_WR, sample);
    if (k == -1) perror("write:");
    printf("ioctl result = %d\n", k); 
    
    printf("\ngetting value of dev_msg and saving it in user_msg thru _IOWR\n");
    k = ioctl(char_driver, SCULL_WR, user_msg);
    if (k == -1) perror("read:");
    printf("kernel said: \"%s\"\n", user_msg);
    printf("ioctl result = %d\n", k); 
}