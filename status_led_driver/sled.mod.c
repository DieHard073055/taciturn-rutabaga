#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xc7dcf939, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0x70e663b6, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0xc996d097, __VMLINUX_SYMBOL_STR(del_timer) },
	{ 0x8574ca6c, __VMLINUX_SYMBOL_STR(gpio_request_array) },
	{ 0x97255bdf, __VMLINUX_SYMBOL_STR(strlen) },
	{ 0x8e360574, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0x4fe38dbd, __VMLINUX_SYMBOL_STR(down_interruptible) },
	{ 0x7eddedb3, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xedfe8af1, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
	{ 0x593a99b, __VMLINUX_SYMBOL_STR(init_timer_key) },
	{ 0x7d11c268, __VMLINUX_SYMBOL_STR(jiffies) },
	{ 0x5f754e5a, __VMLINUX_SYMBOL_STR(memset) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xc0092432, __VMLINUX_SYMBOL_STR(class_unregister) },
	{ 0x5c082778, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0xbe2c0274, __VMLINUX_SYMBOL_STR(add_timer) },
	{ 0x266f9008, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x7afa89fc, __VMLINUX_SYMBOL_STR(vsnprintf) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x126440e4, __VMLINUX_SYMBOL_STR(gpiod_set_raw_value) },
	{ 0x364b3fff, __VMLINUX_SYMBOL_STR(up) },
	{ 0xfe1f32b5, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x9dfdf722, __VMLINUX_SYMBOL_STR(gpio_free_array) },
	{ 0x656198fb, __VMLINUX_SYMBOL_STR(__class_create) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "796A079AB7B20A4687AFA4B");
