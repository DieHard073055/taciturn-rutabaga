/**
 * @file        kbuff_application.c
 * @author      Eshan Shafeeq
 * @date        21 March 2016
 * @version     0.1
 * @brief       An application program to update and read
 *              the buffer of the kernel_buffer character
 *              device in the kernel.
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#define BUFF_LEN    256
#define DEVICE_FILE "/dev/kbuffer"

static char receive[BUFF_LEN];

int main(){
    
    int ret;                    // To store the return values
    int fd;                     // File Pointer
    char data_out[BUFF_LEN];    // Data to be sent to the module

    printf("\tWelcome to the KBUFF APPLICATION\n");

    // Open device file

    fd = open(DEVICE_FILE, O_RDWR);
    if ( fd < 0 ){
        perror("Failed to open the device file, please make sure the\
                device exists\n");
        return errno;
    }

    printf("Please Enter a string to be sent to KBUFF\n");
    scanf("%[^\n]%*c", data_out);
    printf("Writing byte to device file\n");

    ret = write(fd, data_out, strlen(data_out));
    if ( ret < 0 ){
        perror("Failed to write data to the device file\n");
        return errno;
    }
    printf("Press Enter to read data from the device file\n");
    getchar();

    printf("\n\nReading Bytes from device file\n");

    ret = read(fd, receive, BUFF_LEN);
    if ( ret < 0 ){
        perror("Failed to data from the device file\n");
        return errno;
    }

    printf("%d Bytes received from the device file\n", strlen(receive));
    printf("\t-[Data from device file]-\n");
    printf("%s\n\n", receive);
    printf("\t\t[END]\n");

    return 0;
}
