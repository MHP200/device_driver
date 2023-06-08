/**************************************************************
* Class:  CSC-415-03 Spring 2023
* Name: Mahdi Hassanpour
* Student ID: 922278744
* GitHub UserID: MHP200
* Project: Assignment 6 – Device Driver
*
* File: deviceDriver.c
*
* Description: Implements device driver open, read, write, 
*              release and icotl function as well as 
*              starting and ending the driver.
**************************************************************/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

//Init Module
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mahdi Hassanpour");
MODULE_DESCRIPTION("Device Driver");

// Define ioctl commands
#define MY_IOCTL_SET_KEY _IOW('k', 1, int)
#define MY_IOCTL_ENCRYPT _IO('k', 2)
#define MY_IOCTL_DECRYPT _IO('k', 3)

// Define encryption key
static int encryption_key = 0;

// Define encryption and decryption functions
static void encrypt(char *input, char *output)
{
    int i;
    int len = strlen(input);
    
    // Perform encryption algorithm on input and store in output
    for (i = 0; i < len; i++) {
        output[i] = input[i] + encryption_key;
    }
    output[i] = '\0';
}


// Define encryption and decryption functions
static void decrypt(char *input, char *output)
{
    int i;
    int len = strlen(input);
    
    // Perform decryption algorithm on input and store in output
    for (i = 0; i < len; i++) {
        output[i] = input[i] - encryption_key;
    }
    output[i] = '\0';
}


// Define file operations
static int my_open(struct inode *inode, struct file *filp)
{
    // Perform initialization
    printk(KERN_INFO "my_module: device opened\n");
    return 0;
}


// Define file operations
static int my_release(struct inode *inode, struct file *filp)
{
    // Perform cleanup
    printk(KERN_INFO "my_module: device closed\n");
    return 0;
}


// Define file operations
static ssize_t my_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    char encrypted_data[256]; // Assuming the maximum length of encrypted data is 256 characters
    char decrypted_data[256]; // Assuming the maximum length of decrypted data is 256 characters
    int bytes_read = 0;
    
    // Read data from device
    if (count > 256) {
        count = 256;
    }
    if (*f_pos > 0) {
        return 0;
    }
    encrypt("Hello, world!", encrypted_data);
    decrypt(encrypted_data, decrypted_data);
    bytes_read = strlen(decrypted_data);
    
    // Copy data to user-space buffer
    if (copy_to_user(buf, decrypted_data, bytes_read) != 0) {
        return -EFAULT;
    }
    
    // Update file position
    *f_pos += bytes_read;
    
    return bytes_read;
}


// Define file operations
static ssize_t my_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
    char encrypted_data[256]; // Assuming the maximum length of encrypted data is 256 characters
    char decrypted_data[256]; // Assuming the maximum length of decrypted data is 256 characters
    int bytes_written = 0;
    
    // Copy data from user-space buffer
    if (count > 256) {
        count = 256;
    }
    if (copy_from_user(decrypted_data, buf, count) != 0) {
        return -EFAULT;
    }
    
    // Encrypt data
    encrypt(decrypted_data, encrypted_data);
    
    // Write encrypted data to device
    bytes_written = strlen(encrypted_data);
    *f_pos += bytes_written;
    
    return bytes_written;
}


static long my_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
        case MY_IOCTL_SET_KEY:
            // Set encryption key
            break;
        case MY_IOCTL_ENCRYPT:
            // Switch to encryption mode
            break;
        case MY_IOCTL_DECRYPT:
            // Switch to decryption mode
            break;
        default:
            // Handle invalid ioctl command
            break;
    }
        return 0;

}

// Define file operations structure
static struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write,
    .unlocked_ioctl = my_ioctl,
};

// Define device numbers
static dev_t my_dev;

// Define character device structure
static struct cdev *my_cdev;

// Define major and minor numbers
#define MY_MAJOR  415
#define MY_MINOR  0

// Initialization function
static int __init my_init(void)
{
    // Register character device
    if (register_chrdev(MY_MAJOR, "DeviceDriver", &my_fops) < 0) {
        printk(KERN_ALERT "Failed to register character device\n");
        return -1;
    }

    printk(KERN_INFO "my_module loaded\n");
    return 0;
}

// Cleanup function
static void __exit my_exit(void)
{
    // Remove character device
    // cdev_del(my_cdev);

    // Free device numbers
     unregister_chrdev(MY_MAJOR, "DeviceDriver");

    printk(KERN_INFO "my_module unloaded\n");
}

module_init(my_init);
module_exit(my_exit);
