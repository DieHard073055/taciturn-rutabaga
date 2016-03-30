/**
 * @file        timers.c
 * @author      Eshan Shafeeq
 * @date        26 March 2016
 * @version     0.1
 * @brief       A kernel module to learn the basics
 *              of timer interrupts.
 **/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/init.h>

#define DELAY_TIME 50
/*
 * Structure to hold the info
 * about the leds
 */
static struct timer_list interrupt_routine;

/**
 * Functions to print messages to the kernel buffer
 *
 */
static void kern_info(const char *format, ...){
    char msg[256];
    va_list args;
    va_start(args, format);
        vsnprintf(msg, 100, format, args);
    va_end(args);
    printk(KERN_INFO "[TIMERS]: %s\n", msg);
}
static void kern_alert(const char *format, ...){
    char msg[256];
    va_list args;
    va_start(args, format);
        vsnprintf(msg, 100, format, args);
    va_end(args);
    printk(KERN_ALERT "[TIMERS]: %s\n", msg);
}


/**
 * Timer interrupt routine
 * implementation
 *
 */
static void interrupt_routine_function( unsigned long value ){
    //call the apropriate method
    kern_info("routine.. %ld", value);

    interrupt_routine.data = ++value;
    interrupt_routine.expires = jiffies + DELAY_TIME;
    add_timer(&interrupt_routine);
}


/**
 * Module initialization
 *
 */
static int __init timers_init(void){

    kern_info("Module Initializing");

    init_timer(&interrupt_routine);
    interrupt_routine.function = interrupt_routine_function;
    interrupt_routine.data = 0;
    interrupt_routine.expires = jiffies + DELAY_TIME;
    add_timer(&interrupt_routine);
    kern_info("Module initialisation Successful");
    return 0;
}

/**
 * Module termination
 *
 */

static void __exit timers_terminate(void){
    
    del_timer_sync(&interrupt_routine);
    kern_info("Module terminating | Bye bye");
    return;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eshan Shafeeq");
MODULE_DESCRIPTION("Module to learn the timer interrupts");


module_init( timers_init );
module_exit( timers_terminate );
