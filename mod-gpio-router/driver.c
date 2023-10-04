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
#ifndef DRIVER_NAME
#define DRIVER_NAME "rpi-gpio-router"	/* /proc/devices, /proc/modules */
#endif /* DRIVER_NAME */

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

static char *gpi_opt = NULL;
module_param(gpi_opt, charp, S_IRUGO);
MODULE_PARM_DESC(gpi_opt, "GPIO input options");

static bool reverse = false;
module_param(reverse, bool, S_IRUGO);
MODULE_PARM_DESC(reverse, "(optional) reverse mode");

static bool verbose = false;
module_param(verbose, bool, S_IRUGO);
MODULE_PARM_DESC(verbose, "(optional) verbose mode");

struct gpi_desc {
	int R, F, H, L, AR, AF, PU, PD, PN;
};
static void gpi_desc_from_str(struct gpi_desc * d, const char * s) {
	if (!d || !s) return;
	memset(d, 0, sizeof(struct gpi_desc));
	for (; s && *s; s++) {
		if (toupper((int)*s) == 'A') {
			s++;
			if (toupper((int)*s) == 'R') {
				d->AR=1;
			} else if (toupper((int)*s) == 'F') {
				d->AF=1;
			}
		} else if (toupper((int)*s) == 'P') {
			s++;
			if (toupper((int)*s) == 'N') {
				d->PN=1;
			} else if (toupper((int)*s) == 'U') {
				d->PU=1;
			} else if (toupper((int)*s) == 'D') {
				d->PD=1;
			}
		} else if (toupper((int)*s) == 'R') {
			d->R=1;
		} else if (toupper((int)*s) == 'F') {
			d->F=1;
		} else if (toupper((int)*s) == 'H') {
			d->H=1;
		} else if (toupper((int)*s) == 'L') {
			d->L=1;
		}
	}
	return;
}
static void gpi_desc_set(const struct gpi_desc * d, int * dst) {
	if (!d || !dst) return;
	if (d->R) *dst |= IRQF_TRIGGER_RISING;
	if (d->F) *dst |= IRQF_TRIGGER_FALLING;
	if (d->H) *dst |= IRQF_TRIGGER_HIGH;
	if (d->L) *dst |= IRQF_TRIGGER_LOW;
}
static void gpio_trigger_str(char * dst, int gt) {
	if (gt == IRQF_TRIGGER_NONE) strcpy(dst, "NONE");
	else {
		char * s = dst;
		*s = '\0';
		if (gt & IRQF_TRIGGER_RISING) {
			if (*dst != '\0') strcat(s, "|");
			strcat(s, "RISING");
		}
		if (gt & IRQF_TRIGGER_FALLING) {
			if (*dst != '\0') strcat(s, "|");
			strcat(s, "FALLING");
		}
		if (gt & IRQF_TRIGGER_HIGH) {
			if (*dst != '\0') strcat(s, "|");
			strcat(s, "HIGH");
		}
		if (gt & IRQF_TRIGGER_LOW) {
			if (*dst != '\0') strcat(s, "|");
			strcat(s, "LOW");
		}
		if (gt & IRQF_TRIGGER_PROBE) {
			if (*dst != '\0') strcat(s, "|");
			strcat(s, "PROBE");
		}
	}
}

static irqreturn_t gpio_intr(int irq, void *dev_id) {
	int pin_state = gpio_get_value(gpi);
	if (verbose) {
		printk(DRIVER_NAME ": gpio_intr input = %d\n", pin_state);
	}

	if (reverse) {
		gpio_set_value(gpo, pin_state^1);
	} else {
		gpio_set_value(gpo, pin_state);
	}
	return IRQ_HANDLED;
}

static struct gpi_desc gpid;
static char trigger_str[32];

static void print_settings(void) {
	int irq = gpio_to_irq(gpi);

	gpio_trigger_str(trigger_str, gpi_trigger);
	printk(DRIVER_NAME ": gpi=%d irq=%d trigger=%s gpo=%d\n", gpi, irq, trigger_str, gpo);
}

/* At load (insmod) */
static int __init mod_init(void) {
	printk(DRIVER_NAME ": mod_init\n");

	if (gpi_opt != NULL) {
		printk(DRIVER_NAME ": gpi_opt=%s\n", gpi_opt);
		gpi_desc_from_str(&gpid, gpi_opt);
		gpi_trigger = IRQF_TRIGGER_NONE;
		gpi_desc_set(&gpid, &gpi_trigger);
	}

	/* Set `gpo` to output mode. */
	gpio_direction_output(gpo, 1);

	/* Set `gpo` pin-out to 0(Low). */
	gpio_set_value(gpo, 0);

	/* Set `gpi` to input mode. */
	gpio_direction_input(gpi);

	/* Get IRQ number of `gpi`. */
	int irq = gpio_to_irq(gpi);

	print_settings();

	/* Set IRQ handler for `gpi`. */
	if (request_irq(irq, (void*)gpio_intr, IRQF_SHARED | gpi_trigger, "gpio_intr", (void*)gpio_intr) < 0) {
		printk(KERN_ERR "request_irq\n");
		return -1;
	}

	printk(DRIVER_NAME ": insmod done.\n");
	return 0;
}

/* At unload (rmmod) */
static void __exit mod_exit(void) {
	printk(DRIVER_NAME ": mod_exit\n");
	int irq = gpio_to_irq(gpi);
	free_irq(irq, (void*)gpio_intr);
}

module_init(mod_init);
module_exit(mod_exit);
