#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/sysinfo.h>
#include <linux/types.h>
#include <linux/string.h> 
#include <linux/init.h>
#include <linux/proc_fs.h> 
#include <linux/seq_file.h> 
#include <linux/mm.h> 
#include <linux/sched.h>
#include <linux/timer.h> 
#include <linux/jiffies.h> 
#include <linux/tty.h>
#include <linux/sched/signal.h>
#include <linux/sched/mm.h>
#include <linux/binfmts.h>
#include <linux/timekeeping.h>

#define PROC_FILE_NAME "sysinfo_201700633"
#define CGROUP_FILE_PATH_SIZE 256
#define CGROUP_PREFIX "/system.slice/docker-"
#define CGROUP_PREFIX_LEN (sizeof(CGROUP_PREFIX) - 1)
#define MAX_CMDLINE_LENGTH 256

static struct proc_dir_entry *proc_file_entry;

static void *proc_seq_start(struct seq_file *s, loff_t *pos)
{
    static unsigned long counter = 0;

    if (*pos == 0) {
        printk(KERN_INFO "sysinfo_module: start sequence\n");
        return &counter;
    } else {
        return NULL;
    }
}

static void proc_seq_stop(struct seq_file *s, void *v)
{
}

static void *proc_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
    return NULL;
}

static char *get_process_cmdline(struct task_struct *task) {

    /* 
        Creamos una estructura mm_struct para obtener la información de memoria
        Creamos un apuntador char para la línea de comandoscd
        Creamos un apuntador char para recorrer la línea de comandos
        Creamos variables para guardar las direcciones de inicio y fin de los argumentos y el entorno
        Creamos variables para recorrer la línea de comandos
    */
    struct mm_struct *mm;
    char *cmdline, *p;
    unsigned long arg_start, arg_end, env_start;
    int i, len;


    // Reservamos memoria para la línea de comandos
    cmdline = kmalloc(MAX_CMDLINE_LENGTH, GFP_KERNEL);
    if (!cmdline)
        return NULL;

    // Obtenemos la información de memoria
    mm = get_task_mm(task);
    if (!mm) {
        kfree(cmdline);
        return NULL;
    }

    /* 
       1. Primero obtenemos el bloqueo de lectura de la estructura mm_struct para una lectura segura
       2. Obtenemos las direcciones de inicio y fin de los argumentos y el entorno
       3. Liberamos el bloqueo de lectura de la estructura mm_struct
    */
    down_read(&mm->mmap_lock);
    arg_start = mm->arg_start;
    arg_end = mm->arg_end;
    env_start = mm->env_start;
    up_read(&mm->mmap_lock);

    // Obtenemos la longitud de la línea de comandos y validamos que no sea mayor a MAX_CMDLINE_LENGTH - 1
    len = arg_end - arg_start;

    if (len > MAX_CMDLINE_LENGTH - 1)
        len = MAX_CMDLINE_LENGTH - 1;

    // Obtenemos la línea de comandos de  la memoria virtual del proceso
    /* 
        Por qué de la memoria virtual del proceso?
        La memoria virtual es la memoria que un proceso puede direccionar, es decir, la memoria que un proceso puede acceder
    */
    if (access_process_vm(task, arg_start, cmdline, len, 0) != len) {
        mmput(mm);
        kfree(cmdline);
        return NULL;
    }

    // Agregamos un caracter nulo al final de la línea de comandos
    cmdline[len] = '\0';

    // Reemplazar caracteres nulos por espacios
    p = cmdline;
    for (i = 0; i < len; i++)
        if (p[i] == '\0')
            p[i] = ' ';

    // Liberamos la estructura mm_struct
    mmput(mm);
    return cmdline;
}

static int proc_seq_show(struct seq_file *s, void *v)
{
    long mem_util;
    u64 cpu_util;
    struct sysinfo info;
    struct task_struct *task;
    bool first_process = true;  // Variable para manejar la coma entre procesos
    char *cmdline = NULL;
    
    si_meminfo(&info);

    seq_printf(s, "{\n");
    seq_printf(s, "\"total_memory_kb\": %lu,\n", info.totalram * (info.mem_unit / 1024));
    seq_printf(s, "\"free_memory_kb\": %lu,\n", info.freeram * (info.mem_unit / 1024));
    seq_printf(s, "\"used_memory_kb\": %lu,\n", (info.totalram - info.freeram) * (info.mem_unit / 1024));
    printk(KERN_INFO "sysinfo_module: Punto de depuración 1\n");

    seq_printf(s, "\"processes\": [\n");

    for_each_process(task) {
        if (strcmp(task->comm, "containerd-shim") == 0) {
            struct mm_struct *mm = task->mm;
           // char cmdline[256] = "N/A";
            int len = 0;

            printk(KERN_INFO "sysinfo_module: Procesando PID %d\n", task->pid);

            if (!first_process) {
                seq_printf(s, ",\n");  // Imprimir coma antes del siguiente elemento, si no es el primero
            }
            cmdline = get_process_cmdline(task);
            // if (mm) {
            //     len = snprintf(cmdline, sizeof(cmdline), "%s", task->comm);
            //     seq_printf(s, "{ \"cmdline\": \"%s\",\n", cmdline);
            // } else {
            //     seq_printf(s, "{ \"cmdline\": \"N/A\",\n");
            //     printk(KERN_WARNING "Process %d has no memory management info.\n", task->pid);
            // }
            seq_printf(s, "   { \"Cmdline\": \"%s\",\n", cmdline ? cmdline : "N/A");

            seq_printf(s, "\"pid\": %d,\n", task->pid);
            seq_printf(s, "\"name\": \"%s\",\n", task->comm);
            seq_printf(s, "\"vsz_kb\": %lu,\n", mm ? (mm->total_vm << (PAGE_SHIFT - 10)) : 0);

            unsigned long rss_kb = 0;
            if (mm) {
                rss_kb = (get_mm_rss(mm) << (PAGE_SHIFT - 10));
            } else {
                printk(KERN_WARNING "Process %d has no RSS info.\n", task->pid);
            }
            seq_printf(s, "\"rss_kb\": %lu,\n", rss_kb);
            
            mem_util = (rss_kb * 100) / ((info.totalram * info.mem_unit) >> 10);  // Utilización de memoria en porcentaje
            cpu_util = task->utime + task->stime; // Usar tiempo de usuario y sistema
            seq_printf(s, "\"Memoria_Utilizada\": %lu ,\n", mem_util);
            seq_printf(s, "\"Cpu_Utilizado\": %llu \n", cpu_util);

            // Leer el archivo cgroup para obtener el ID del contenedor
            // Aquí debes añadir el código para manejar el ID del contenedor si lo has habilitado

            seq_printf(s, "}");
            first_process = false;  // No es el primer proceso después de la primera iteración
        }
    }
    seq_printf(s, "\n");

    seq_printf(s, "]\n");

    seq_printf(s, "}\n");

    return 0;
}

static const struct seq_operations proc_seq_ops = {
    .start = proc_seq_start,
    .stop = proc_seq_stop,
    .next = proc_seq_next,
    .show = proc_seq_show,
};

static int proc_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &proc_seq_ops);
}

static const struct proc_ops proc_file_ops = {
    .proc_open = proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = seq_release,
};

static int __init my_module_init(void)
{
    proc_file_entry = proc_create(PROC_FILE_NAME, 0, NULL, &proc_file_ops);
    if (!proc_file_entry) {
        return -ENOMEM;
    }
    printk(KERN_INFO "Module loaded. /proc/%s created.\n", PROC_FILE_NAME);
    return 0;
}

static void __exit my_module_exit(void)
{
    proc_remove(proc_file_entry);
    printk(KERN_INFO "Module unloaded. /proc/%s removed.\n", PROC_FILE_NAME);
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RODRIGOCARCUZ");
MODULE_DESCRIPTION("Modulo de kernel para capturar metricas");
