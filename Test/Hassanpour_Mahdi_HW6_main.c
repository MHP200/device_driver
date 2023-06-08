/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Name: Mahdi Hassanpour
* Student ID: 922278744
* GitHub UserID: MHP200
* Project: Assignment 6 – Device Driver
*
* File: 
*
* Description: Implements device driver open, read, write, 
*              release and icotl function as well as 
*              starting and ending the driver.
**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>


#define DEVICE "/dev/deviceDriver"
#define WRITEVALUE _IOW('a', 'a', int)
#define READVALUE _IOR('a', 'b', int)

int main() {
    int file_desc, value;
    char buffer[1024];

    file_desc = open(DEVICE, O_RDWR);

    if (file_desc < 0) {
        printf("Failed to open the device file!\n");
        return -1;
    }

    printf("Enter a string to encrypt: ");
    scanf("%[^\n]%*c", buffer);

    write(file_desc, buffer, strlen(buffer));

    printf("Enter a value to write to the device: ");
    scanf("%d", &value);

    ioctl(file_desc, WRITEVALUE, &value);

    read(file_desc, buffer, 1024);

    printf("Encrypted string received from device: %s\n", buffer);

    ioctl(file_desc, READVALUE, &value);

    printf("Value read from the device: %d\n", value);

    close(file_desc);

    return 0;
}
