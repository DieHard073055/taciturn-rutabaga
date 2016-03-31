/**
 * @file    printops.h
 * @author  Eshan Shafeeq
 * @version 0.1
 * @date    25 March 2016
 * @brief   This file contains some easy functions
 *          to print out messages to the kernel log
 **/

#include <linux/kernel.h>
#include <linux/module.h>
#ifndef _PRINTOPS_H_
#define _PRINTOPS_H_

/**
 * kern_info
 *
 * @brief   Prints a message to the kernel buffer
 *          with the message priority of KERN_INFO.
 *          Usually for debugging purposes.
 *
 **/

static void kern_info(  int len, const char *format, ...){
    char    msg[256];
    va_list args;

    va_start( args, format );
        vsnprintf(msg, strlen( format ) + len + 1, format, args );
    va_end( args );
    printk(KERN_INFO "[CMDDEV]: %s\n", msg);
}
/**
 * kern_alert
 *
 * @brief   Prints a message to the kernel buffer
 *          with a higher priority. Usually for
 *          alert messages
 *
 **/

static void kern_alert(  int len, const char *format, ...){
    char    msg[256];
    va_list args;

    va_start( args, format );
        vsnprintf(msg, strlen( format ) + len + 1, format, args );
    va_end( args );
    printk(KERN_ALERT "[CMDDEV]: %s\n", msg);
}
#endif
