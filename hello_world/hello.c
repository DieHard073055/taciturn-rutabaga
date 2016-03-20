/*
 * =====================================================================================
 *
 *       Filename:  hello.c
 *
 *    Description: Kernel module to test the linux headers
 *    			   and the make file. 
 *
 *        Version:  1.0
 *        Created:  20/03/16 22:28:47
 *       Revision:  none
 *       Compiler:  kbuild
 *
 *         Author:  Eshan Shafeeq 
 *   Organization:  Diehardofdeath
 *
 * =====================================================================================
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init hello_init(void){
	printk(KERN_INFO "HELLOMOD : Module initialized\n");
	return 0;
}

static void __exit hello_exit(void){
	printk(KERN_INFO "HELLOMOD : Module exit\n");
}

module_init(hello_init);
module_exit(hello_exit);
