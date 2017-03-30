#include <stdio.h>  
#include <stdlib.h>  
#include <fcntl.h>  
#include <unistd.h>
#include <string.h>

int char_driver;  
const size_t max_length = 1UL << 22;

int main(void) { 
    
    char_driver = open("/dev/char_driver", O_RDWR, 0644);  
    ssize_t rc;
 
    if (char_driver == -1) {  
        printf("unable to open char_driver.");  
        exit(EXIT_FAILURE);  
    }
    
    char *long_string = (char *)calloc(max_length, sizeof(char));
    char *head_pointer = long_string;
    char *tail_pointer = long_string;
    
    head_pointer += 4;
    tail_pointer += (max_length - 4);
    
    if (long_string == 0) {
        printf("ERROR: Out of memory\n");
        exit(EXIT_FAILURE);
    }
    
    memcpy(long_string, "HEAD", 4);
    memset(head_pointer, 'x', max_length - 4);
    memcpy(tail_pointer, "TAIL", 4);
    
    rc = write(char_driver, long_string, max_length);
    printf("Return code of write function is %zu.\n", rc);
    
    free(long_string);
    close(char_driver);  
    return 0;  
} 
