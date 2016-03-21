#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init hello_init_module(void){
	printk(KERN_INFO "Module initialized\n");
	return 0;
}

static void __exit hello_exit_module(void){
	printk(KERN_INFO "Module exit\n");
}
module_init(hello_init_module);
module_exit(hello_exit_module);
