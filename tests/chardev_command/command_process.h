/**
 * @file    process_command.h
 * @author  Eshan Shafeeq
 * @version 0.1
 * @date    31 March 2016
 * @brief   This file is to process commands received 
 *          from the user through character device file
 *          and assign the appropriate function based on
 *          the command parameters.
 **/


#include <linux/kernel.h>
#include <linux/module.h>
#include "printops.h"
#include "interrupt.h"

#ifndef _PROCESSCOMMAND_H_
#define _PROCESSCOMMAND_H_

/**
 * Validate buffer
 *
 * @brief   This function validates the buffer received
 *          from the user. Makes sure only white space
 *          and numeric values are given as command.
 *
 * @param   buff        The buffer received from the user
 * @param   buff_len    The length of the buffer
 * @param   buff_i      The value of buff[i] converted to
 *                      ACII value.
 * @param   space       To keep count of the white space 
 * @param   num         To keep count of the numeric digits
 *
 **/
static inline bool validate_buffer( const char *buff, size_t buff_len ){
    size_t i;
    int buff_i = 0;
    short space=0;
    short num=0;

    for( i=0; i<buff_len-1; i++ ){
        buff_i = buff[i] - '0';
        //debug
        //kern_info( 0, "processing #%i : %c - %d", i, buff[i], buff_i );
        //
        if( buff_i == -16 || buff_i > -1 ){
            if( buff_i == -16 ) 
                space++;
            if( buff_i > -1 && buff_i < 10 ) 
                num++;
        }else{
            return false;
        }
    }
    if( space == 2 && num == 3 )
        return true;
    else
        return false;
}

/**
 * Process Command
 *
 * @brief   This function processes the command
 *          received by the buffer and executes
 *          the proper action.
 *
 * @param   buff        The buffer received from the user
 * @param   buff_len    The length of the buffer
 * @param   color       To store the color selected by the user
 * @param   delay       To store the delay length selected by the user
 * @param   qty         To store the number of blinks selected
 *
 **/
static void process_command( const char *buff, size_t buff_len ){
//    size_t i;
    short color;
    short delay;
    short qty;

    if( validate_buffer( buff, buff_len ) ){
        kern_info( buff_len-2, "buffer : %s", buff);
        /*  --DEBUG--
         *  kern_info(0, "     index: char|ascii|int cast");
            kern_info(0, "     --------------------------");
        for( i=0; i<buff_len-1; i++ ){
            kern_info(0, "analysis #%i:  %c | %d  | %d", i, buff[i], buff[i], buff[i] - '0');
        }*/

        color = buff[0] - '0';
        delay = buff[2] - '0';
        qty   = buff[4] - '0';

        switch(color){
            case RED:
                kern_info( 0, "RED color selected");
                break;
            case GREEN:
                kern_info( 0, "GREEN color selected");
                break;
            case BLUE:
                kern_info( 0, "BLUE color selected");
                break;
        }
        switch(delay){
            case SHORT:
                kern_info( 0, "SHORT time delay selected");
                break;
            case NORMAL:
                kern_info( 0, "NORMAL time delay selected");
                break;
            case LONG:
                kern_info( 0, "LONG time delay selected");
                break;
        }

        kern_info( 0, "QTY : %d", qty );
        start_timer_interrupt(color, delay, qty);
        
    }

}

#endif
