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

/* default value */
#define GPIO_PIN_OUTPUT 4
#define GPIO_PIN_INPUT 16
#define GPI_TRIGGER (IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING)

static int gpo = GPIO_PIN_OUTPUT;
module_param(gpo, int, S_IRUGO);
MODULE_PARM_DESC(gpo, "GPIO output number");

static int gpi = GPIO_PIN_INPUT;
module_param(gpi, int, S_IRUGO);
MODULE_PARM_DESC(gpi, "GPIO input number");

static int gpi_trigger = GPI_TRIGGER;

static char *gpi_opt = "";
module_param(gpi_opt, charp, S_IRUGO);
MODULE_PARM_DESC(gpi_opt, "GPIO input options");

static bool reverse = false;
module_param(reverse, bool, S_IRUGO);
MODULE_PARM_DESC(reverse, "(optional) reverse mode");

static bool verbose = false;
module_param(verbose, bool, S_IRUGO);
MODULE_PARM_DESC(verbose, "(optional) verbose mode");

static irqreturn_t gpio_intr(int irq, void *dev_id) {
	int btn;

	btn = gpio_get_value(gpi);
	if (verbose) {
		printk(DRIVER_NAME ": gpio_intr input = %d\n", btn);
	}

	if (reverse) {
		gpio_set_value(gpo, btn^1);
	} else {
		gpio_set_value(gpo, btn);
	}
	return IRQ_HANDLED;
}

/* At load (insmod) */
static int mod_init(void) {
	printk(DRIVER_NAME ": mod_init\n");

	/* Set `gpo` to output mode. */
	gpio_direction_output(gpo, 1);

	/* Set `gpo` pin-out to 0(Low). */
	gpio_set_value(gpo, 0);

	/* Set `gpi` to input mode. */
	gpio_direction_input(gpi);

	/* Get IRQ number of `gpi`. */
	int irq = gpio_to_irq(gpi);
	printk(DRIVER_NAME ": gpio_to_irq = %d\n", irq);

	/* Set IRQ handler for `gpi`. */
	if (request_irq(irq, (void*)gpio_intr, IRQF_SHARED | gpi_trigger, "gpio_intr", (void*)gpio_intr) < 0) {
		printk(KERN_ERR "request_irq\n");
		return -1;
	}

	return 0;
}

/* At unload (rmmod) */
static void mod_exit(void) {
	printk(DRIVER_NAME ": mod_exit\n");
	int irq = gpio_to_irq(gpi);
	free_irq(irq, (void*)gpio_intr);
}

module_init(mod_init);
module_exit(mod_exit);
