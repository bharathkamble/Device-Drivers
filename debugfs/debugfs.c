#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>

#define MODULE_NAME "debugfs_example"

static struct dentry *dir, *file;
static int debugfs_value = 0; // Value exposed via debugfs

/* Read function */
static ssize_t debugfs_read(struct file *fp, char __user *user_buffer, size_t count, loff_t *position) {
    char buffer[64];
    int len = snprintf(buffer, sizeof(buffer), "%d\n", debugfs_value);

    return simple_read_from_buffer(user_buffer, count, position, buffer, len);
}

/* Write function */
static ssize_t debugfs_write(struct file *fp, const char __user *user_buffer, size_t count, loff_t *position) {
    char buffer[64];

    if (count >= sizeof(buffer))
        return -EINVAL;

    if (copy_from_user(buffer, user_buffer, count))
        return -EFAULT;

    buffer[count] = '\0';
    if (kstrtoint(buffer, 10, &debugfs_value))
        return -EINVAL;

    return count;
}

/* File operations structure */
static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = debugfs_read,
    .write = debugfs_write,
};

/* Module initialization */
static int __init debugfs_example_init(void) {
    dir = debugfs_create_dir(MODULE_NAME, NULL); // Create directory in /sys/kernel/debug
    if (!dir) {
        pr_err("Failed to create debugfs directory\n");
        return -ENOMEM;
    }

    file = debugfs_create_file("value", 0644, dir, NULL, &fops); // Create file
    if (!file) {
        pr_err("Failed to create debugfs file\n");
        debugfs_remove_recursive(dir);
        return -ENOMEM;
    }

    pr_info("debugfs_example module loaded\n");
    return 0;
}

/* Module exit */
static void __exit debugfs_example_exit(void) {
    debugfs_remove_recursive(dir); // Remove all debugfs entries
    pr_info("debugfs_example module unloaded\n");
}

module_init(debugfs_example_init);
module_exit(debugfs_example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bharath");
MODULE_DESCRIPTION("A simple debugfs example");

