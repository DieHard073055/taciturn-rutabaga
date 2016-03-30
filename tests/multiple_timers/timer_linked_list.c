/**
 * @file        timer_linked_list.c
 * @author      Eshan Shafeeq
 * @date        27 March 2016
 * @version     0.1
 * @brief       A kernel module to learn timer interrupts
 *              with a linked list.
 **/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/random.h>
#include <linux/slab.h>
#include <linux/list.h>

#define DELAY_TIME 50
/*
 * Structure for the timer interrupt
 */
static struct timer_list interrupt_routine;

/**
 * Structure for the linked list
 **/

struct led_state {
    int     id;
    short   color;
    short   delay;
    bool    state;

    struct list_head list;
};

struct led_state led_list;
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
    struct led_state *state_it;
    size_t i=0;
    kern_info("routine.. %ld", value);
    
    list_for_each_entry( state_it, &(led_list.list), list ){
        if( i == value ){
            kern_info("id : %d", state_it->id);
            kern_info("color : %hu", state_it->color);
            kern_info("delay : %hu", state_it->delay);
            kern_info("-----------");
        }
        ++i;
        interrupt_routine.data = ++value;
        interrupt_routine.expires = jiffies + DELAY_TIME;
        add_timer(&interrupt_routine);
    }
        
}


/**
 * Module initialization
 *
 */
static int __init timers_init(void){

    struct led_state *new_state;
   // struct led_state *state_it;
    size_t i=0;
    
    kern_info("Module Initializing");
    
    INIT_LIST_HEAD(&(led_list.list));

    //list setup
    for ( i=0; i<10; i++ ){
        new_state = kmalloc( sizeof(*new_state), GFP_KERNEL );
        new_state->id = i;
        get_random_bytes(&(new_state->color), sizeof(short));
        get_random_bytes(&(new_state->delay), sizeof(short));
        new_state->state = false;
        INIT_LIST_HEAD(&(new_state->list));
        list_add_tail(&(new_state->list), &(led_list.list));
    }
    
    //iterate through the list
    kern_info("Iterating through the list");

    /*list_for_each_entry( state_it, &(led_list.list), list ){
        kern_info("id : %d", state_it->id);
        kern_info("color : %hu", state_it->color);
        kern_info("delay : %hu", state_it->delay);
        kern_info("---------");
    }*/

    //Timer setup
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
    struct led_state    *state_it;
    struct led_state    *tmp;

    //remove timer
    del_timer_sync(&interrupt_routine);

    //remove list elements
    list_for_each_entry_safe(state_it, tmp, &(led_list.list), list){
        kern_info("destroying element with id : %d", state_it->id);
        list_del(&(state_it->list));
        kfree(state_it);
    }

    kern_info("Module terminating | Bye bye");
    return;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eshan Shafeeq");
MODULE_DESCRIPTION("Module to learn the timer interrupts");


module_init( timers_init );
module_exit( timers_terminate );
