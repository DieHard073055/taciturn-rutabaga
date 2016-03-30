/**
 * @file    linked_list.c
 * @author  Eshan Shafeeq
 * @version 0.1
 * @date    24 March 2016
 * @brief   This module demonstrates the use of
 *          linked list in kernel space.
 **/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/random.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Demonstration of linked list");
MODULE_AUTHOR("Eshan Shafeeq");

struct led_state {
    int     id;
    short   color;
    short   length;
    bool   state;

    struct list_head list;
};

struct led_state led_state_list;

int __init led_state_init(void){
    
    struct led_state *new_state;
    struct led_state *a_state;
    size_t i;
    
    printk(KERN_INFO "MODULE STARTED\n");
    INIT_LIST_HEAD(&led_state_list.list);
    
    printk(KERN_INFO "Initializing the linked list\n");
    for(i=0; i<10; i++){
        new_state = kmalloc(sizeof(*new_state), GFP_KERNEL);
        new_state->id = i;
        get_random_bytes(&(new_state->color), sizeof(short));
        get_random_bytes(&(new_state->length), sizeof(short));
        new_state->state = false;
        INIT_LIST_HEAD(&new_state->list);
        list_add_tail(&(new_state->list), &(led_state_list.list));
    }

    printk(KERN_INFO "Iterating through the list\n");

    list_for_each_entry(a_state, &(led_state_list.list), list){
        printk(KERN_INFO "<ID : %d | color : %hu | length : %hu>\n",
                a_state->id, a_state->color, a_state->length);   
    }
    printk(KERN_INFO "\n");

    return 0;
}

void __exit led_state_terminate(void){
    struct led_state *it_ls;
    struct led_state *tmp;

    printk(KERN_INFO "MODULE UNLOADING\n");
    printk(KERN_INFO "Destruction of the linked list\n");

    list_for_each_entry_safe(it_ls, tmp, &(led_state_list.list), list){

        printk(KERN_INFO "Destruction of the node with id : %d", it_ls->id);
        list_del(&(it_ls->list));
        kfree(it_ls);
    }

    return;
}

module_init(led_state_init);
module_exit(led_state_terminate);
