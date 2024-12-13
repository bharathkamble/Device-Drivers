#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BHarath");
MODULE_DESCRIPTION("Sysfs example for controlling GPIO");
MODULE_VERSION("1.0");

#define LED_GPIO 529  // GPIO pin for the LED

// Sysfs attributes
static struct kobject *example_kobject;
static int led_state = 0;

// GPIO initialization
static int gpio_init(void) {
    int ret;

    ret = gpio_request(LED_GPIO, "sysfs_led");
    if (ret) {
        pr_err("Failed to request GPIO %d\n", LED_GPIO);
        return ret;
    }

    ret = gpio_direction_output(LED_GPIO, 0);
    if (ret) {
        pr_err("Failed to set GPIO %d as output\n", LED_GPIO);
        gpio_free(LED_GPIO);
        return ret;
    }

    pr_info("GPIO %d initialized as output\n", LED_GPIO);
    return 0;
}

// GPIO cleanup
static void gpio_cleanup(void) {
    gpio_set_value(LED_GPIO, 0); // Turn off LED
    gpio_free(LED_GPIO);
    pr_info("GPIO %d freed\n", LED_GPIO);
}

// Sysfs attribute: LED state
static ssize_t led_state_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", led_state);
}

static ssize_t led_state_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    int value;

    if (kstrtoint(buf, 10, &value) != 0 || (value != 0 && value != 1)) {
        pr_err("Invalid value, please write 0 or 1\n");
        return -EINVAL;
    }

    led_state = value;
    gpio_set_value(LED_GPIO, led_state);
    pr_info("LED state set to %d\n", led_state);

    return count;
}

static struct kobj_attribute led_state_attribute =
    __ATTR(led_state, 0660, led_state_show, led_state_store);

// Initialize module
static int __init sysfs_example_init(void) {
    int ret;
  
    pr_info("Initializing sysfs example module\n");

    ret = gpio_init();
    if (ret) {
        pr_err("Failed to initialize GPIO\n");
        return ret;
    }

    // Create sysfs kobject
    example_kobject = kobject_create_and_add("sys_fs", kernel_kobj);
    if (!example_kobject) {
        pr_err("Failed to create sysfs kobject\n");
        gpio_cleanup();
        return -ENOMEM;
    }

    // Create sysfs file
    ret = sysfs_create_file(example_kobject, &led_state_attribute.attr);
    if (ret) {
        pr_err("Failed to create sysfs file\n");
        kobject_put(example_kobject);
        gpio_cleanup();
        return ret;
    }

    pr_info("Sysfs example module initialized\n");
    return 0;
}

// Exit module
static void __exit sysfs_example_exit(void) {
    pr_info("Exiting sysfs example module\n");

    sysfs_remove_file(example_kobject, &led_state_attribute.attr);
    kobject_put(example_kobject);
    gpio_cleanup();

    pr_info("Sysfs example module exited\n");
}

module_init(sysfs_example_init);
module_exit(sysfs_example_exit);
