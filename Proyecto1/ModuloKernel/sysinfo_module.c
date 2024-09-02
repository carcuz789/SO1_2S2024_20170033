#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/string.h>

#define PROC_FILE_NAME "sysinfo_201700633"
#define CGROUP_FILE_PATH_SIZE 256
#define CGROUP_PREFIX "/system.slice/docker-"
#define CGROUP_PREFIX_LEN (sizeof(CGROUP_PREFIX) - 1)

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

static int proc_seq_show(struct seq_file *s, void *v)
{
    long mem_util;
    u64 cpu_util;
    // char cgroup_file_path[CGROUP_FILE_PATH_SIZE];
    // char cgroup_data[CGROUP_FILE_PATH_SIZE];
    // struct file *file;
    // loff_t pos;
    // ssize_t read_size;
    struct sysinfo info;
    struct task_struct *task;
    
    si_meminfo(&info);

    seq_printf(s, "{\n");
    seq_printf(s, "\"total_memory_kb\": %lu,\n", info.totalram * (info.mem_unit / 1024));
    seq_printf(s, "\"free_memory_kb\": %lu,\n", info.freeram * (info.mem_unit / 1024));
    seq_printf(s, "\"used_memory_kb\": %lu,\n", (info.totalram - info.freeram) * (info.mem_unit / 1024));
    printk(KERN_INFO "sysinfo_module: Punto de depuración 1\n");

    seq_printf(s, "\"processes\": [\n");

    for_each_process(task) {
        struct mm_struct *mm = task->mm;
        char cmdline[256] = "N/A";
        int len = 0;

        printk(KERN_INFO "sysinfo_module: Procesando PID %d\n", task->pid);

        if (mm) {
            len = snprintf(cmdline, sizeof(cmdline), "%s", task->comm);
            seq_printf(s, "{ \"cmdline\": \"%s\",\n", cmdline);
        } else {
            seq_printf(s, "{ \"cmdline\": \"N/A\",\n");
            printk(KERN_WARNING "Process %d has no memory management info.\n", task->pid);
        }

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
        seq_printf(s, "\"Memoria Utilizada\": \"%ld %%\",\n", mem_util);
        seq_printf(s, "\"Cpu Utilizado\": \"%llu\",\n", cpu_util);

        // Leer el archivo cgroup para obtener el ID del contenedor
        // snprintf(cgroup_file_path, CGROUP_FILE_PATH_SIZE, "/proc/%d/cgroup", task->pid);

        // file = filp_open(cgroup_file_path, O_RDONLY, 0);
        // if (IS_ERR(file)) {
        //     seq_printf(s, "\"container_id\": \"N/A\",\n");
        //     continue;
        // }

        // pos = 0;
        // read_size = kernel_read(file, cgroup_data, CGROUP_FILE_PATH_SIZE - 1, &pos);
        // filp_close(file, NULL);

        // if (read_size > 0) {
        //     cgroup_data[read_size] = '\0';
        //     char *start = strstr(cgroup_data, CGROUP_PREFIX);
        //     if (start) {
        //         start += CGROUP_PREFIX_LEN;
        //         char *end = strchr(start, '.');
        //         if (end) *end = '\0';
        //         seq_printf(s, "\"container_id\": \"%s\",\n", start);
        //     } else {
        //         seq_printf(s, "\"container_id\": \"N/A\",\n");
        //     }
        // } else {
        //     seq_printf(s, "\"container_id\": \"N/A\",\n");
        // }

        seq_printf(s, "},\n");
    }
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
