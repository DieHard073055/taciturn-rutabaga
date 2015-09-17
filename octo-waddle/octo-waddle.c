#include <linux/init.h>
#include <linux/module.h>
/*
Author Eshan Shafeeq
Linux Kernel Module : Octo-Waddle

This is my hello world equivalent introduction to 
linux kernel module programming
*/

static int Octo_waddle_init(void){
	printk(KERNEL_INFO "OCTO WADDLE!!!");
	return 0;
}

static void Octo_waddle_end(void){
	printk(KERNEL_INFO "OCTO WADDLE : BYE BYE!");
}

module_init(Octo_waddle_init);
module_exit(Octo_waddle_end);
