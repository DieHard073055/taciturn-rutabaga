#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/semaphore.h>
#include "printops.h"

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#define DELAY_TIME 50

static struct timer_list interrupt;
static struct semaphore running;

static void interrupt_routine( unsigned long value ){
    
    kern_info( 0, "routine %ld", value );
    interrupt.data = ++value;
    interrupt.expires = jiffies + DELAY_TIME;
    if( value < 10 ){
        add_timer( &interrupt );
    }else{
        up( &running );
    }

}
static bool start_timer_interrupt(void){
    if( down_interruptible(&running) == 0 ){
        interrupt.data = 0;
        interrupt.expires = jiffies + DELAY_TIME;
        add_timer( &interrupt );
        return true;
    }else{
        return false;
    }

}
static void setup_timer_interrupt(void){

    kern_info( 0, "Setting up timer interrupt" );
    sema_init( &running, 1 ); 
    init_timer( &interrupt );
    interrupt.function= interrupt_routine;
    start_timer_interrupt();
}

static void remove_timer(void){
    
    kern_info( 0, "Removing timer interrupt");

    del_timer_sync( &interrupt );
}
#endif
