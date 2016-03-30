/**
 * @file    status_led_driver.c
 * @author  Eshan Shafeeq
 * @date    22/03/2016
 * @version 0.1
 * @brief   A character device driver to control
 *          an rgb led attached to the pi. Which
 *          helps in indicating the status for 
 *          multiple applications.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/gpio.h>
#include <linux/list.h>
#include <linux/slab.h>

#define DEVICE_NAME "sled"
#define CLASS_NAME  "sled_class"

#define COLOR_RED       20
#define COLOR_GREEN     21
#define COLOR_BLUE      22

#define DELAY_SHORT     10
#define DELAY_NORMAL    50
#define DELAY_LONG      100

//Module Description
//------------------
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eshan Shafeeq");
MODULE_DESCRIPTION("Character Device driver\
        to help control an rgb status led");
MODULE_VERSION("0.1");
MODULE_SUPPORTED_DEVICE(DEVICE_NAME);

//TODO
//Module Variables
//----------------
//@param major_number : To store the major number
//                      assigned to the module.
//@param open_counter : To keep count of the number 
//                      of times the device has been
//                      opened.
//@param sled_class  : To hold the registered class
//                      structure reference.
//@param sled_device : To hold the registered device
//                      structure reference.
static int major_number;
static int open_counter=0;
static struct class* sled_class = NULL;
static struct device* sled_device = NULL;

struct led_state {
    int     id;
    short   color;
    short   delay;
    bool    state;

    struct list_head list;
};
/*static struct gpio leds[] = {
    {   4,  GPIOF_OUT_INIT_HIGH, "REDLED"       },
    {   17, GPIOF_OUT_INIT_HIGH, "GREENLED"     },
    {   27, GPIOF_OUT_INIT_HIGH, "BLUELED"      }
};
*/
struct led_state state_list;
static struct timer_list task_list;
static bool timer_state;

//Prototype Functions
//-------------------
static int      device_open(struct inode *, struct file *);
static int      device_release(struct inode *, struct file *);
static ssize_t  device_read(struct file *, char *, size_t, loff_t *);
static ssize_t  device_write(struct file *, const char *, size_t, loff_t *);

//File Operations Structure
//-------------------------
static struct file_operations fops = {
    .open       =   device_open,
    .release    =   device_release,
    .read       =   device_read,
    .write      =   device_write
};

//Print Operations to the kernel buffer
//-------------------------------------
static void kern_info(int len, const char *format, ...){ 
    char msg[256];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, strlen(format)+len+1, format, args);
    va_end(args);
    printk(KERN_INFO "[SLED] : %s\n", msg);
}
static void kern_alert(int len, const char *format, ...){
    char msg[256];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, strlen(format)+len+1, format, args);
    va_end(args);
    printk(KERN_ALERT "[SLED] : %s\n", msg);
}

/**
 * @brief   Module initialization function
 *          - Obtain a major number for this
 *          driver.
 *          -Create a class for this device.
 *          -Register this device in the kernel
 *         
 */

static int __init sled_driver_init(void){
    kern_info(0, "Module Initializing");
    
    // Dynamically obtain a major number

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if ( major_number < 0 ){
        kern_alert(0, "Failed to obtain a MAJOR_NUMBER");
        return major_number;
    }
    
    kern_info(3, "Obtained a major number of %d", major_number);
    
    // Register the device class

    sled_class = class_create(THIS_MODULE, CLASS_NAME);
    if ( IS_ERR(sled_class) ){
        unregister_chrdev(major_number, DEVICE_NAME);
        kern_alert(0, "Failed to register device class");
        return PTR_ERR(sled_class);
    }
    
    kern_info(0, "Successfully Registered the Device Class");

    // Registering the device driver

    sled_device = device_create(sled_class, NULL, MKDEV(major_number, 0),
            NULL, DEVICE_NAME);
    if ( IS_ERR(sled_device) ){
        class_destroy(sled_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        kern_alert(0, "Failed to create and register the device");
        return PTR_ERR(sled_device);
    }

    kern_info(0, "Successfully Registered the Device");
    
    // Initialize the list
    INIT_LIST_HEAD( &(state_list.list) );
    timer_state = false;
    kern_info(0, "State list initiated");
    return 0;
}

static void __exit sled_driver_exit(void){
    // Remove the device

    device_destroy(sled_class, MKDEV(major_number, 0));

    //Remove the class
    
    class_unregister(sled_class);
    class_destroy(sled_class);

    // Remove the major number

    unregister_chrdev(major_number, DEVICE_NAME);

    kern_info(0, "Module Exit | Bye Bye");   
}

//Timer interrupt routine
static void interrupt_routine( unsigned long value ){
    struct led_state *state_it;
    struct led_state *tmp;
    size_t i=0;
    bool in = false;

    kern_info( 3, "--- In the routine #%ld ---", value );

    list_for_each_entry( state_it, &(state_list.list), list ){
        if( i==value ){
            kern_info( 3, "COLOR : %hu", state_it->color );
            kern_info( 3, "DELAY : %hu", state_it->delay );
            kern_info( 3, "STATE : %s\n\n", state_it->state ? "true" : "false" );
            in = true;
            
            //TODO turn on off led
            task_list.data = ++value;
            task_list.expires = jiffies + state_it->delay;
            add_timer( &task_list );
        }

        ++i;
    }
    if( in != true ){
        //remove timer
        del_timer_sync( &task_list );

        //remove list elements
        list_for_each_entry_safe( state_it, tmp, &(state_list.list), list ){
            kern_info( 3, "Destroying element with id %d", state_it->id );
            list_del( &(state_it->list) );
            kfree( state_it );
        }

        timer_state = false;
    }
}

//device open
//-----------
static int device_open(struct inode *ptr_inode, struct file *ptr_file){
    kern_info(0, "Device has been opened %d times", open_counter++);
    return 0;
}

//device release
//--------------
static int device_release(struct inode *ptr_inode, struct file *ptr_file){
    kern_info(0, "Device has been successfully closed");
    return 0;
}

//device read
//-----------
static ssize_t device_read(struct file *ptr_file, char *buffer, 
        size_t buff_len, loff_t *offset){
   /* int ret;
    ret = copy_to_user(buffer, kbuff, kbuff_len);
    if (ret){
        kern_alert("Failed to send data to user");
        return -EFAULT;
    }*/
    return 0; 
}
//Function to compare two strings
//-------------------------------
static int string_cmp(const char* str, const char* cmp, int len){
    size_t i;
    for( i=0; i<len; i++ )
        kern_info(0, "str[%i] = %c", i, str[i]);
    for( i=0; i<len; i++ ){
        kern_info(0, "string compare | %c == %c", str[i], cmp[i]);
        if( str[i] != cmp[i] ){
            return -1;
        }
    }
    return 0;
}

//Function to count the number of characters in a string
//------------------------------------------------------
static int strchrcnt(const char* str, int len, char c){
    int i;
    int count =0;
    if ( len < 1 ){ return 0 ; }
    for ( i=0; i<len; i++){
        if ( str[i] == c ){
            ++count;
        }
    }
    return count;
}

//To validate if a given string contains the command format
//---------------------------------------------------------
static bool legit_command(const char* str, int len, char c){
    short segment_counter = 0;
    bool last_was_letter = false;
    int i;

    for ( i=0; i<len; i++ ){
        if(str[i] != c){
            if ( last_was_letter != true ){
                segment_counter++;
                last_was_letter = true;
            }
        }else{
            if( last_was_letter ){
                last_was_letter = false;
            }
        }
    }
    if ( segment_counter == 3 ){
        return true;
    }else{
        return false;
    }
}

static struct led_state * create_new_state(int i, short c, short d, bool s){
    struct led_state *new_state;

    new_state = kmalloc( sizeof(*new_state), GFP_ATOMIC );
    new_state->id = i;
    new_state->color = c;
    new_state->delay = d;
    new_state->state = s;
    
    return new_state;

}
static void create_task_list(short color, short delay, unsigned int qty){
    size_t i;
    struct led_state *new_state;

    for( i=0; i<qty; i++ ){
        new_state = create_new_state(i, color, delay, true);
        INIT_LIST_HEAD(&(new_state->list));
        list_add_tail( &(new_state->list), &(state_list.list) );

        new_state = create_new_state(i, color, delay, false);
        INIT_LIST_HEAD(&(new_state->list));
        list_add_tail( &(new_state->list), &(state_list.list) );
    }


}
//Break down the verified string into its command pieces and 
//execute the appropriate operations for it.
//----------------------------------------------------------
static void process_command(const char *buffer, size_t buff_len){
    char *data_in=(char*) buffer;
    char *token;
    char color[6]={' '};
    char length[6]={' '};
    char qty[4]={' '};
    unsigned int iqty;
    bool failed = false;
    
    short s_color=0;
    short s_delay=0;
    
    kern_info(buff_len-3, "buffer :_%s_", buffer);
    strncpy(data_in, buffer, buff_len);

    token = strsep(&data_in, " ");   
    strcpy(color, token);
    kern_info( 6, "unfiltered color : _%s_", color );

    token = strsep(&data_in, " ");   
    strcpy(length, token);
    kern_info( 6, "unfiltered delay : _%s_", length );
    
    token = strsep(&data_in, " ");   
    strcpy(qty, token);
    kern_info( 2, "unfiltered qty : _%s_", qty );

    

    if ( strcmp( color, "RED" )==0 ){
        kern_info(0, "RED color selected");
        s_color=COLOR_RED;
    }else if ( string_cmp( color, "GREEN", strlen("GREEN")+1 )==0 ){
        kern_info(0, "GREEN color selected");
        s_color=COLOR_GREEN;
    }else if ( string_cmp( color, "BLUE", strlen("BLUE")+1 )==0 ){
        kern_info(0, "BLUE color selected");
        s_color=COLOR_BLUE;
    }else{
        failed = true;
        kern_alert( 0, "Failed to detect color");
    }
    
    if ( failed != true ){
        if ( strcmp( length, "SHORT" )==0 ){
            kern_info( 0, "SHORT length selected" );
            s_delay=DELAY_SHORT;
        }else if ( strcmp( length, "NORMAL" )==0 ){
            kern_info(0, "NORMAL length selected");
            s_delay=DELAY_NORMAL;
        }else if ( strcmp( length, "LONG" )==0 ){
            kern_info(0, "LONG length selected");
            s_delay=DELAY_LONG;
        }else{
            failed = true;
        }
    }else{
        kern_alert( 0, "delay was ignored");
    }
    if ( failed != true ){   
        if ( kstrtouint( qty, 0, &iqty ) == 0 ){
            
            if( iqty < 11 ){
                kern_info( 0, "BLINK quantity is %i times", iqty );
            }else{
                kern_info( 0, "BLINK quantity is too large! [ %i > 11 ]", iqty );
                failed=true;
            }
        }else{
            failed = true;
            kern_alert( 0, "failed to convert qty : %s", qty);
        }
    }else{
        kern_alert( 0, "failed to check qty" );
    }
    if( failed != true && timer_state != true ){
        //execute the operations
        kern_info(0, "Everything was successful");
        create_task_list( s_color, s_delay, iqty );

        init_timer( &task_list );
        task_list.function = interrupt_routine;
        task_list.data = 0;
        task_list.expires = jiffies + DELAY_SHORT;
        add_timer( &task_list );
        timer_state = true;
        kern_info( 0, "Task list has been initiated");
               
    }

}


//device write
//------------
static ssize_t device_write(struct file *ptr_file, const char *buffer,
        size_t buff_len, loff_t *offset){
    
    int len_count =  strchrcnt(buffer, buff_len, ' ');
    kern_info(3, "Received %lu characters from the user", buff_len);
    kern_info(buff_len-3, "%s", buffer);
    kern_info(1, "Number of white space : %d", len_count);
    
    if(len_count == 2){
        if( legit_command(buffer, buff_len, ' ')){
            len_count++;
            process_command(buffer, buff_len);
        }
     }


    return buff_len;
}

module_init(sled_driver_init);
module_exit(sled_driver_exit);
