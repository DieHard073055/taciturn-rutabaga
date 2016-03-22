/*
 * @file    hello.c
 * @author  Eshan Shafeeq
 * @date    21/03/2016
 * @version 0.1
 * @brief   A simple example hello world kernel module driver
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eshan Shafeeq");
MODULE_DESCRIPTION("Simple hello world module");
MODULE_VERSION("0.1");

//Module Entry Function
//---------------------
static int __init hello_init_module(void){
	printk(KERN_INFO "Module initialized\n");
	return 0;
}

//Module Exit Function
//--------------------
static void __exit hello_exit_module(void){
	printk(KERN_INFO "Module exit\n");
}


module_init(hello_init_module);
module_exit(hello_exit_module);
