/**
 * @file    led_gpio.h
 * @author  Eshan Shafeeq
 * @version 0.1
 * @date    31 March 2016
 * @brief   This file is to give access to 
 *          gpio pins to control leds.
 **/

#include <linux/gpio.h>
#include "printops.h"

#ifndef _LED_GPIO_H_
#define _LED_GPIO_H_

/**
 * Structure to hold the array of
 * leds with initial state
 **/

static struct gpio leds[] = {
    {   4,  GPIOF_OUT_INIT_HIGH, "REDLED"    },
    {   17, GPIOF_OUT_INIT_HIGH, "GREENLED"  },
    {   27, GPIOF_OUT_INIT_HIGH, "BLUELED"   }
};

/**
 * To know whether the gpio's
 * have been assigned
 **/

static bool initialized;

/**
 * Initialization function
 * 
 * @brief   Requests for the required
 *          array of gpios.
 * @param   ret     To store the state of
 *                  the request function.
 **/

static inline void initiate_leds( void ){
    int ret =0;
    
    ret = gpio_request_array( leds, ARRAY_SIZE( leds ) );
    if( ret < 0 ){
        kern_alert( 0, "Failed to initialize leds" );
        initialized = false;
        return;
    }

    kern_info( 0, "Leds have been initialised Succesfully" );
    initialized = true;

}

/**
 * Destructor
 * 
 * @brief   Release the gpio pins which
 *          was requested previously.
 **/

static inline void release_leds( void ){
    size_t i;

    for( i=0; i<ARRAY_SIZE( leds ); i++ ){
        gpio_set_value( leds[i].gpio, 0 );
    }

    gpio_free_array( leds, ARRAY_SIZE( leds ) );
    kern_info( 0, "Leds have been released" );
    initialized = false;
}

/**
 * Toggle led
 *
 * @brief   This function sets a given led on or off
 *
 * @param   index   The index of the led to be toggled
 * @param   value   The boolean state that the led should have.
 *
 **/

static inline void toggle_led( size_t index, bool value ){

    //kern_info( 5, "index %d state %s", index, value?"true":"false" );
    if( value ){
        gpio_set_value( leds[ index ].gpio, 0 );
    }else{
        gpio_set_value( leds[ index ].gpio, 1 );
    }
}

#endif
