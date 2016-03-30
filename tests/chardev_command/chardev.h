#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include "printops.h"
#include "command_process.h"

#ifndef _CHARDEV_H_
#define _CHARDEV_H_


#define DEVICE_NAME     "chardev_command"
#define DEVICE_CLASS    "chardev_command_class"


/**
 * Module Global Variables
 * -----------------------
 *  @param  major_number        : To store the major number assigned
 *                                  to this device;
 *  @param  cmd_buff            : A buffer to store the command from
 *                                  the user.
 *  @param  cmd_buff_len        : To store the length of the buffer
 *                                  recieved from the user.
 *  @param  cmd_device_class    : To store the device class structure.
 *  @param  cmd_device          : To store the registered device structure.
 *
 **/

static int      major_number;
//static char     cmd_buff;
//static short    cmd_buff_length;

static struct   class*  cmd_device_class    = NULL;
static struct   device* cmd_device          = NULL;

/**
 * Prototype Functions [FOPS]
 * --------------------------
 **/

static int      device_open(    struct inode *,
                                struct file * );
static int      device_release( struct inode *,
                                struct file * );
static ssize_t  device_read(    struct file *,
                                char   *,
                                size_t,
                                loff_t * );
static ssize_t  device_write(   struct file *,
                                const char *,
                                size_t,
                                loff_t * );

/**
 * File Operations Structure
 * -------------------------
 **/
static struct file_operations fops = {
    .open       =   device_open,
    .release    =   device_release,
    .read       =   device_read,
    .write      =   device_write
};

/**
 * Char device Open
 * ----------------
 **/
static int device_open( struct inode *ptr_inode, struct file *ptr_file ){
    kern_info( 0, "Device has been opened");
    return 0;
}

/**
 * Char device Release
 * -------------------
 **/
static int device_release( struct inode *ptr_inode, struct file *ptr_file ){
    kern_info( 0, "Device has been released");
    return 0;
}

/**
 * Char device Read
 * ----------------
 **/
static ssize_t device_read( struct file *ptr_file, char *buff, 
                            size_t buff_len, loff_t *offset ){
    return 0;
}

/**
 * Char device Write
 * -----------------
 **/
static ssize_t device_write( struct file *ptr_file, const char *buff,
                             size_t buff_len,       loff_t *offset ){
    kern_info( 0, "Recieved %d bytes from user", buff_len );
    process_command( buff, buff_len );
    return buff_len;
}


/**
 * Setup character device function
 * -------------------------------
 **/
static int setup_chardev(void){
    kern_info( 0, "Character device setup");
    
    //Major number setup
    major_number = register_chrdev( 0, DEVICE_NAME, &fops );
    if( major_number < 0 ){
        kern_alert( 0, "Failed to obtain a MAJOR_NUMBER");
        return major_number;
    }

    kern_info( 3, "Successfully Obtained the MAJOR_NUMBER : %d", major_number );

    //Device class setup
    cmd_device_class = class_create( THIS_MODULE, DEVICE_CLASS );
    if( IS_ERR( cmd_device_class ) ){
        unregister_chrdev( major_number, DEVICE_NAME );
        kern_alert( 0, "Failed to register the DEVICE CLASS");
        return PTR_ERR( cmd_device_class );
    }

    kern_info( 0, "Successfully Registered the DEIVCE CLASS");

    //Deivce Registration setup
    cmd_device = device_create( cmd_device_class, NULL, MKDEV( major_number, 0 ),
                                NULL,                              DEVICE_NAME );
    if( IS_ERR( cmd_device ) ){
        class_destroy( cmd_device_class );
        unregister_chrdev( major_number, DEVICE_NAME );
        kern_alert( 0, "Failed to register device");
        return PTR_ERR( cmd_device );
    }

    kern_info( 0, "Successfully Registered the DEVICE");

    return 0;
}

/**
 * Remove the character device functionality
 * -----------------------------------------
 **/
static void remove_chardev(void){
    
    kern_info( 0, "Character device removal");

    //Remove the device
    device_destroy( cmd_device_class, MKDEV( major_number, 0) );

    //Remove class
    class_unregister( cmd_device_class );
    class_destroy( cmd_device_class );

    //Unregbister Major number
    unregister_chrdev( major_number, DEVICE_NAME );
}
#endif
