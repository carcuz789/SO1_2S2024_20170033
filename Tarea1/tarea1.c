#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/sched/signal.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tu Nombre");
MODULE_DESCRIPTION("Un módulo de kernel que imprime información del sistema");

static int __init info_module_init(void) {
    struct sysinfo i;
    struct task_struct *task;

    // Obtener información de la RAM
    si_meminfo(&i);

    printk(KERN_INFO "Total RAM: %lu kB\n", i.totalram * 4);
    printk(KERN_INFO "Free RAM: %lu kB\n", i.freeram * 4);

    // Imprimir procesos
    printk(KERN_INFO "Procesos:\n");
    for_each_process(task) {
        if (task->parent)
            printk(KERN_INFO "Padre PID: %d, Hijo PID: %d\n", task->parent->pid, task->pid);
    }

    return 0;
}

static void __exit info_module_exit(void) {
    printk(KERN_INFO "Módulo removido\n");
}

module_init(info_module_init);
module_exit(info_module_exit);
