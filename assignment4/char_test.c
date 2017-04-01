#include <stdio.h>  
#include <stdlib.h>  
#include <fcntl.h>  
#include <unistd.h>
#include <string.h>
#include <linux/errno.h>

int char_driver;  
const size_t four_meg_length = 1UL << 22;
const size_t four_meg_plus_one_length = (1UL << 22) + 1;
const size_t five_meg_length = (1UL << 22) + (1UL << 20);
void info_message();

int main(int argc, char **argv) { 
    ssize_t copied_bytes;
    size_t max_length;
    
    if (argc == 1 || argc > 3) {
        info_message();
        exit(EXIT_FAILURE);
    }
    
    if (!(strcmp(argv[1], "--size") || strcmp(argv[1], "-S"))) {
        info_message();
        exit(EXIT_FAILURE);
    }
    
    if (!strcmp(argv[2], "5")) 
        max_length = five_meg_length;
    else if (!strcmp(argv[2], "4")) 
        max_length = four_meg_length;
    else if (!strcmp(argv[2], "41")) 
        max_length = four_meg_plus_one_length;
    else {
        info_message();
        exit(EXIT_FAILURE);
    }
   
    char_driver = open("/dev/char_driver", O_RDWR, 0644);  
 
    if (char_driver == -1) {  
        printf("unable to open char_driver.");  
        exit(-EACCES);  
    }
    
    char *long_string = (char *)calloc(max_length, sizeof(char));
    if (long_string == 0) {
        printf("ERROR: Out of memory\n");
        exit(-ENOMEM);
    }
    
    char *head_pointer = long_string;
    char *tail_pointer = long_string;
    
    head_pointer += 4;
    tail_pointer += (max_length - 4);
    
    memcpy(long_string, "HEAD", 4); /*set HEAD context at the head of the 
                                     * pointer to the data */
    memset(head_pointer, '\0', max_length - 4); /*padding with a null character
                                                 * so that head and tail 
                                                 * contexts can be made visible
                                                 * to the user */
    memcpy(tail_pointer, "TAIL", 4); /* set TAIL context at the end of the 
                                      * pointer to the data (hence -4) */
    
    copied_bytes = write(char_driver, long_string, max_length);
    printf("Number of bytes copied is %zu.\n", copied_bytes);
    
    free(long_string);
    close(char_driver); 
    
    return 0;  
} 

void info_message(void) {
    printf("\n");
    printf("Simple testbench to test char driver writes.\n");
    printf("4MB and 5MB copies are implemented in this testbench.\n");
    printf("Additional 4MB + 1 byte is also used in this testbench.\n");
    printf("\nAllowed values:\n");
    printf("\t-S, --size\n");
    printf("\t\t4 to pass 4 MB\n");
    printf("\t\t5 to pass 5 MB\n");
    printf("\t\t41 to pass 4 MB plus one byte\n");
    printf("\n");
}