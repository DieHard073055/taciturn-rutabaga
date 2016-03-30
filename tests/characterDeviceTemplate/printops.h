#include <linux/kernel.h>
#include <linux/module.h>
#ifndef _PRINTOPS_H_
#define _PRINTOPS_H_
/**
 * Print Opertations to the Kernel Buffer
 * --------------------------------------
 **/
static void kern_info(  int len, const char *format, ...){
    char    msg[256];
    va_list args;

    va_start( args, format );
        vsnprintf(msg, strlen( format ) + len + 1, format, args );
    va_end( args );
    printk(KERN_INFO "[CMDDEV]: %s\n", msg);
}
static void kern_alert(  int len, const char *format, ...){
    char    msg[256];
    va_list args;

    va_start( args, format );
        vsnprintf(msg, strlen( format ) + len + 1, format, args );
    va_end( args );
    printk(KERN_ALERT "[CMDDEV]: %s\n", msg);
}
#endif
