/**
 * @file    kernel_buffer.c
 * @author  Eshan Shafeeq
 * @date    21/03/2016
 * @version 0.1
 * @brief   A character device driver to implement the fops
 *          structure, communication between user space and
 *          kernel space, Dynamic allocation of major number
 *          and registration of a character device.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "kbuffer"
#define CLASS_NAME  "kbuffClass"

//Module Description
//------------------
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eshan Shafeeq");
MODULE_DESCRIPTION("Character Device Driver to hold a   \
        buffer of bytes which can be read again from    \
        user space");
MODULE_VERSION("0.1");
//MODULE_SUPPORTED_DEVICE(DEVICE_NAME);

//Module Variables
//----------------
//@param major_number : To store the major number
//                      assigned to the module.
//@param kbuffer      : To store the buffer of bytes
//                      received from the user.
//@param kbuff_len    : To store the size of kbuffer
//@param open_counter : To keep count of the number 
//                      of times the device has been
//                      opened.
//@param kbuff_class  : To hold the registered class
//                      structure reference.
//@param kbuff_device : To hold the registered device
//                      structure reference.
static int major_number;
static char kbuff[256] = {0};
static short kbuff_len;
static int open_counter=0;

static struct class* kbuff_class = NULL;
static struct device* kbuff_device = NULL;

//Prototype Functions
//-------------------
static int      device_open(struct inode *, struct file *);
static int      device_release(struct inode *, struct file *);
static ssize_t  device_read(struct file *, char *, size_t, loff_t *);
static ssize_t  device_write(struct file *, const char *, size_t, loff_t *);

//File Operations Structure
//-------------------------
static struct file_operations fops = {
    .open       =   device_open,
    .release    =   device_release,
    .read       =   device_read,
    .write      =   device_write
};

//Print Operations to the kernel buffer
//-------------------------------------
static void kern_info(const char *format, ...){ 
    char msg[256];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, 100, format, args);
    va_end(args);
    printk(KERN_INFO "[KBUFF] : %s\n", msg);
}
static void kern_alert(const char *format, ...){
    char msg[256];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, 100, format, args);
    va_end(args);
    printk(KERN_ALERT "[KBUFF] : %s\n", msg);
}
/**
 * @brief   Module initialization function
 * <give a brief overview of the steps in the
 * function>
 */

static int __init kernel_buffer_init(void){
    kern_info("Module Initializing");
    
    // Dynamically obtain a major number

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if ( major_number < 0 ){
        kern_alert("Failed to obtain a MAJOR_NUMBER");
        return major_number;
    }
    
    kern_info("Obtained a major number of %d", major_number);
    
    // Register the device class

    kbuff_class = class_create(THIS_MODULE, CLASS_NAME);
    if ( IS_ERR(kbuff_class) ){
        unregister_chrdev(major_number, DEVICE_NAME);
        kern_alert("Failed to register device class");
        return PTR_ERR(kbuff_class);
    }
    
    kern_info("Successfully Registered the Device Class");

    // Registering the device driver

    kbuff_device = device_create(kbuff_class, NULL, MKDEV(major_number, 0),
            NULL, DEVICE_NAME);
    if ( IS_ERR(kbuff_device) ){
        class_destroy(kbuff_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        kern_alert("Failed to create and register the device");
        return PTR_ERR(kbuff_device);
    }

    kern_info("Successfully Registered the Device");
    kern_info("Please create a device node inorder to begin communication");
    kern_info("sudo mknod /dev/kbuffer -m 666 c %d 1", major_number);

    return 0;
}

static void __exit kernel_buffer_exit(void){
    // Remove the device

    device_destroy(kbuff_class, MKDEV(major_number, 0));

    //Remove the class
    
    class_unregister(kbuff_class);
    class_destroy(kbuff_class);

    // Remove the major number

    unregister_chrdev(major_number, DEVICE_NAME);

    kern_info("Module Exit | Bye Bye");   
}

//device open
//-----------
static int device_open(struct inode *ptr_inode, struct file *ptr_file){
    kern_info("Device has been opened %d times", open_counter++);
    return 0;
}

//device release
//--------------
static int device_release(struct inode *ptr_inode, struct file *ptr_file){
    kern_info("Device has been successfully closed");
    return 0;
}

//device read
//-----------
static ssize_t device_read(struct file *ptr_file, char *buffer, 
        size_t buff_len, loff_t *offset){
    int ret;
    ret = copy_to_user(buffer, kbuff, kbuff_len);
    if (ret){
        kern_alert("Failed to send data to user");
        return -EFAULT;
    }
    return 0; 
}

//device write
//------------
static ssize_t device_write(struct file *ptr_file, const char *buffer,
        size_t buff_len, loff_t *offset){
    sprintf(kbuff, "%s :- %d bytes", buffer, buff_len);
    kbuff_len = strlen(kbuff);

    kern_info("Received %lu characters from the user", buff_len);

    return buff_len;
}

module_init(kernel_buffer_init);
module_exit(kernel_buffer_exit);
