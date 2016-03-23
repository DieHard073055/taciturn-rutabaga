#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/time.h>
#include <linux/init.h>

// Struct to define the led
static struct gpio leds[] = {
    {   4,  GPIOF_OUT_INIT_LOW, "LED 1"},
    {   17, GPIOF_OUT_INIT_LOW, "LED 2"},
    {   27, GPIOF_OUT_INIT_LOW, "LED 3"},
    {   22, GPIOF_OUT_INIT_LOW, "LED 3"}
};


//Module initlization
static int __init led_init(void){
    int ret=0;
    int i=0;
    ret = gpio_request_array(leds, ARRAY_SIZE(leds));
    if ( ret < 0 ){
        printk(KERN_ALERT "LED: Unable to request for leds\n");
        return ret;
    }
    printk(KERN_INFO "LED: Leds have been successfully initialized\n");

    for ( i=0; i<ARRAY_SIZE(leds); i++ ){
        gpio_set_value(leds[i].gpio, 1);
    }
    return 0;
}

//Module terminate
static void __exit led_terminate(void){
    int i;

    for( i=0; i<ARRAY_SIZE(leds); i++){
        gpio_set_value(leds[i].gpio, 0);
    }

    gpio_free_array(leds, ARRAY_SIZE(leds));
}

MODULE_LICENSE("GPL");

module_init(led_init);
module_exit(led_terminate);
