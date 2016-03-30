/**
 * @file    chardev_command.c
 * @author  Eshan Shafeeq
 * @date    30 March 2016
 * @version 0.1
 * @brief   A character device driver capable
 *          of receiving commands and distinguishing
 *          them and giving outputs based on the
 *          commands received.
 **/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include "chardev.h"

/**
 * Module definitions
 * ------------------
 **/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eshan Shafeeq");
MODULE_DESCRIPTION("Device driver to recieve commands and execute them");
MODULE_VERSION("0.1");
MODULE_SUPPORTED_DEVICE(DEVICE_NAME);

/**
 * Module Initialization
 * ---------------------
 **/
static int __init cmd_dev_init(void){
    int ret;

    ret = setup_chardev();
    if( ret != 0 ){
        return ret;
    }

    return 0;
}

/**
 * Module Termination
 * ------------------
 **/
static void __exit cmd_dev_exit(void){
    remove_chardev();      
}

module_init( cmd_dev_init );
module_exit( cmd_dev_exit );

