#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x6d26b3a7, "seq_printf" },
	{ 0x441c5e2e, "init_task" },
	{ 0x656e4a6e, "snprintf" },
	{ 0x1e6d26a8, "strstr" },
	{ 0xbcab6ee6, "sscanf" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x737c5c97, "proc_remove" },
	{ 0x2fa57770, "seq_read" },
	{ 0x9fd18288, "seq_lseek" },
	{ 0x499b0a0b, "seq_release" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x6e2d7b65, "proc_create" },
	{ 0x122c3a7e, "_printk" },
	{ 0x552a4084, "seq_open" },
	{ 0x40c7247c, "si_meminfo" },
	{ 0x708cd699, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "9D07080DE5E3392E85EA65B");
