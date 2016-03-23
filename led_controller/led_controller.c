/**
 * @file        led_controller.c
 * @author      Eshan Shafeeq
 * @date        22 March 2016
 * @version     0.1
 * @brief       A kernel module to control 4 leds connected
 *              to the gpio header on a raspberry-pi B+, 
 *              Using the linux timer interrupts.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/gpio.h>

#define DELAY_TIME 50
/*
 * Structure to hold the info
 * about the leds
 */
static struct gpio leds[] = {
    {   4,  GPIOF_OUT_INIT_LOW, "LED1" },
    {   17, GPIOF_OUT_INIT_LOW, "LED2" },
    {   27, GPIOF_OUT_INIT_LOW, "LED3" },
    {   22, GPIOF_OUT_INIT_LOW, "LED4" }
};

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
    printk(KERN_INFO "[LEDCONTROLLER]: %s\n", msg);
}
static void kern_alert(const char *format, ...){
    char msg[256];
    va_list args;
    va_start(args, format);
        vsnprintf(msg, 100, format, args);
    va_end(args);
    printk(KERN_ALERT "[LEDCONTROLLER]: %s\n", msg);
}


/**
 * Timer interrupt routine
 * implementation
 *
 */
static void interrupt_routine_function( unsigned long value ){
    //call the apropriate method
    switch(value){
        case 0:
            gpio_set_value(leds[0].gpio, 1);
            gpio_set_value(leds[1].gpio, 0);
            gpio_set_value(leds[2].gpio, 0);
            gpio_set_value(leds[3].gpio, 0);
            break;

        case 1:
            gpio_set_value(leds[0].gpio, 0);
            gpio_set_value(leds[1].gpio, 1);
            gpio_set_value(leds[2].gpio, 0);
            gpio_set_value(leds[3].gpio, 0);
            break;

        case 2:
            gpio_set_value(leds[0].gpio, 0);
            gpio_set_value(leds[1].gpio, 0);
            gpio_set_value(leds[2].gpio, 1);
            gpio_set_value(leds[3].gpio, 0);
            break;

        case 3:
            gpio_set_value(leds[0].gpio, 0);
            gpio_set_value(leds[1].gpio, 0);
            gpio_set_value(leds[2].gpio, 0);
            gpio_set_value(leds[3].gpio, 1);
            break;
    }

    interrupt_routine.data = ++value;
    if(value == 4)
        interrupt_routine.data = 0;
    interrupt_routine.expires = jiffies + (DELAY_TIME);
    add_timer(&interrupt_routine);
}


/**
 * Module initialization
 *
 */
static int __init led_controller_init(void){
    
    int ret=0;

    kern_info("Module Initializing");
    ret = gpio_request_array( leds, ARRAY_SIZE(leds) );
    if ( ret ){
        kern_alert("Uable to request for leds!");
        return ret;
    }

    init_timer(&interrupt_routine);
    interrupt_routine.function = interrupt_routine_function;
    interrupt_routine.data = 0;
    interrupt_routine.expires = jiffies + (DELAY_TIME);
    add_timer(&interrupt_routine);
    kern_info("Module initialisation Successful");
    return 0;
}

/**
 * Module termination
 *
 */

static void __exit led_controller_terminate(void){
    
    del_timer_sync(&interrupt_routine);
    gpio_set_value(leds[0].gpio, 0);
    gpio_free_array( leds, ARRAY_SIZE(leds) );
    kern_info("Module terminating | Bye bye");
    return;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eshan Shafeeq");
MODULE_DESCRIPTION("Module to control 4 status leds for the pi B+");


module_init( led_controller_init );
module_exit( led_controller_terminate );
