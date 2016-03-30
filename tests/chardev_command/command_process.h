#include <linux/kernel.h>
#include <linux/module.h>
#include "printops.h"

#ifndef _PROCESSCOMMAND_H_
#define _PROCESSCOMMAND_H_

#define RED     3
#define GREEN   4
#define BLUE    5

#define SHORT   6
#define NORMAL  7
#define LONG    8

static bool validate_buffer( const char *buff, size_t buff_len ){
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
static void process_command( const char *buff, size_t buff_len ){
    size_t i;
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
    }

}

#endif
