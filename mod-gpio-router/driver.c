#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");
#define DRIVER_NAME "rpi-gpio-router"	/* /proc/devices */

#define GPIO_PIN_OUTPUT 42
#define GPIO_PIN_INPUT 16

static irqreturn_t gpio_intr(int irq, void *dev_id) {
	int btn;
	printk(DRIVER_NAME ": gpio_intr\n");

	btn = gpio_get_value(GPIO_PIN_INPUT);
	printk(DRIVER_NAME ": button = %d\n", btn);

	gpio_set_value(GPIO_PIN_OUTPUT, btn^1);
	return IRQ_HANDLED;
}

/* At load (insmod) */
static int mod_init(void) {
	printk(DRIVER_NAME ": mod_init\n");

	/* Set GPIO_PIN_OUTPUT to output mode. */
	gpio_direction_output(GPIO_PIN_OUTPUT, 1);
	/* Set GPIO_PIN_OUTPUT pin-out to 0(Low). */
	gpio_set_value(GPIO_PIN_OUTPUT, 0);

	/* Set GPIO_PIN_INPUT to input mode. */
	gpio_direction_input(GPIO_PIN_INPUT);

	/* Get IRQ number of GPIO_PIN_INPUT. */
	int irq = gpio_to_irq(GPIO_PIN_INPUT);
	printk("gpio_to_irq = %d\n", irq);

	/* Set IRQ handler for GPIO_PIN_INPUT. */
	if (request_irq(irq, (void*)gpio_intr, IRQF_SHARED | IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "gpio_intr", (void*)gpio_intr) < 0) {
		printk(KERN_ERR "request_irq\n");
		return -1;
	}

	return 0;
}

/* At unload (rmmod) */
static void mod_exit(void) {
	printk(DRIVER_NAME ": mod_exit\n");
	int irq = gpio_to_irq(GPIO_PIN_INPUT);
	free_irq(irq, (void*)gpio_intr);
}

module_init(mod_init);
module_exit(mod_exit);
