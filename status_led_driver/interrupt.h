/**
 * @file    interrupt.h
 * @author  Eshan Shafeeq
 * @version 0.1
 * @date    31 March 2016
 * @brief   This file implements the timer
 *          interrupt function to blink an
 *          led the requested amount of times
 *          concurrently. It uses a semaphore
 *          to prevent crashes.
 *
 **/
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/semaphore.h>
#include <linux/list.h>
#include <linux/slab.h>
#include "printops.h"
#include "led_gpio.h"

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#define     DELAY_TIME      50
#define     SHORT_DELAY     10
#define     NORMAL_DELAY    50
#define     LONG_DELAY      100

#define     RED             3
#define     GREEN           4
#define     BLUE            5

#define     SHORT           6
#define     NORMAL          7
#define     LONG            8

/**
 * Globals
 *
 * @param   interrupt   The timer_list structure to 
 *                      interrupt regularly.
 * @param   running     The semaphore to prevent access
 *                      to the timer start function.
 * @param   state_list  The structure to hold the led
 *                      states over time.
 * @param   state_len   To store the size of the state
 *                      list.
 *
 **/

static struct timer_list interrupt;
static struct semaphore running;

struct led_state {
    int     id;
    short   color;
    short   delay;
    bool    state;

    struct list_head head;
};
struct led_state state_list;
static int state_len;

/**
 * Create new Task
 *
 * @brief   To dynamically allocate space for 
 *          a new led_state object and return
 *          the address to the caller.
 *
 * @param   i           Id of the newly created object
 * @param   c           Color of the newly created object
 * @param   d           Delay time of newly created object
 * @param   s           State of the newly created object
 * @param   new_state   The pointer to access the newly created object
 *
 **/
static struct led_state * create_new_task(int i, short c, short d, bool s){
    
    struct led_state *new_state;

    new_state = kmalloc( sizeof( *new_state ), GFP_KERNEL );
    new_state->id       = i;
    new_state->color    = c;

    switch( d ){
        
        case SHORT:
            new_state->delay    = SHORT_DELAY;
            break;
        case NORMAL:
            new_state->delay    = NORMAL_DELAY;
            break;
        case LONG:
            new_state->delay    = LONG_DELAY;
            break;

    }
    new_state->state    = s;
    
    INIT_LIST_HEAD( &new_state->head );
    state_len++;

    return new_state;

}

/**
 * Get Delay
 *
 * @brief   obtain the delay amount from the
 *          object recieved via the value index.
 *
 * @param   value   index of the state_list object which
 *                  the delay time should be obtained from.
 *
 **/

static short get_delay( unsigned long value ){
    struct led_state *state;
    list_for_each_entry( state, &(state_list.head), head ){
        if( value==state->id ){
            return state->delay;
        }
    }
    return DELAY_TIME;
}

/**
 * Create a task list
 *
 * @brief       Create a linkedlist of led_state type objects.
 *
 * @param   color       Color variable of all the objects in the list.
 * @param   delay       Delay time for all the objects
 * @param   qty         Number of blinks for the specific list.
 * @param   new_state   The pointer to hold a reference to newly
 *                      created object.
 *
 **/
static void create_task_list( short color, short delay, short qty ){
    
    struct led_state *new_state;
    size_t i;

    kern_info( 0, "Initializing task list" );

    INIT_LIST_HEAD( &state_list.head );
    state_len=0;

    //Populate the list
    for( i=0; i<(qty*2); i+=2 ){
        new_state = create_new_task(i, color, delay, true);
        list_add_tail( &(new_state->head), &(state_list.head) );
        
        new_state = create_new_task(i+1, color, delay, false);
        list_add_tail( &(new_state->head), &(state_list.head) );
    }


}

/**
 * Destroy Task List
 *
 * @brief   Release the memory dynamically allocated
 *          to the state list once it is not required.
 *
 * @param   state_iter      A pointer variable to iterate the list
 * @param   state_iter_tmp  A pointer variable for safe iteration
 *
 **/

static void destroy_task_list( void ){
    struct led_state *state_iter;
    struct led_state *state_iter_tmp;

    list_for_each_entry_safe( state_iter, state_iter_tmp, &(state_list.head), head ){
        list_del( &(state_iter->head) );
        kfree( state_iter );
    }
}

/**
 * Trigger Led
 *
 * @brief   This function triggers the selected
 *          led based on the number of blinks.
 *
 * @param   state   A pointer to obtain the values
 *                  from the chosen state.
 * @param   value   The current index of the chosen
 *                  object from the linked list.
 *
 **/
static void trigger_led( unsigned long value ){
    struct led_state *state;

    list_for_each_entry( state, &(state_list.head), head ){
        if( value==state->id ){
            //kern_info( 0, "color : %hu", state->color );
            //kern_info( 0, "delay : %hu", state->delay );
            //kern_info( 0, "state : %hu", state->state );
            switch( state->color ){
                case RED:
                    toggle_led( 0, state->state );
                    break;
                case GREEN:
                    toggle_led( 1, state->state );
                    break;
                case BLUE:
                    toggle_led( 2, state->state );
                    break;

            }
        }
    }
}

/**
 * Interrupt Routine
 *
 * @brief   This is the interrupt routine which
 *          happen based on the delay time
 *          requested by the user.
 *
 * @param   value   The count of the current iteration.
 *                  also to keep track of the current
 *                  state from the state list.
 *
 **/
static void interrupt_routine( unsigned long value ){
    
    //kern_info( 0, "routine %ld", value );
    trigger_led( value ); 
    interrupt.data = ++value;
    interrupt.expires = jiffies + get_delay( value );
    if( value < state_len ){
        add_timer( &interrupt );
    }else{
        destroy_task_list();
        release_leds();
        up( &running );
        kern_info( 0, "Timer stopped");
    }

}

/**
 * State Timer Interrupt
 *
 * @brief   The function which starts all the magic
 *
 * @param   color   The color obtained from the command.
 * @param   delay   The delay time obtained from the command.
 * @param   qty     The blink amount obtained from the command.
 *
 **/

static bool start_timer_interrupt( short color, short delay, short qty ){
    kern_info( 0, "Timer started");
    if( down_interruptible(&running) == 0 ){
        interrupt.data = 0;
        
        create_task_list( color, delay, qty );
        initiate_leds();

        interrupt.expires = jiffies + DELAY_TIME;
        add_timer( &interrupt );
        return true;
    }else{
        return false;
    }

}
/**
 *  Setup Timer Interrupts
 *
 *  @brief  Initiates the semaphore and the interrupt function.
 *
 **/
static void setup_timer_interrupt(void){

    kern_info( 0, "Setting up timer interrupt" );
    sema_init( &running, 1 ); 
    init_timer( &interrupt );
    interrupt.function= interrupt_routine;
}

/**
 *  Remove Timer
 *
 *  @brief  Function to end all the magic happening.
 *
 **/
static void remove_timer(void){
    
    kern_info( 0, "Removing timer interrupt");

    del_timer_sync( &interrupt );
}
#endif
