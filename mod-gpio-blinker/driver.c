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
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");
#define DRIVER_NAME "rpi-gpio-blinker"	/* /proc/devices, /proc/modules */

/* default value */
#define GPIO_PIN_OUTPUT 42
#define PERIOD_SEC 0
#define PERIOD_NSEC 1000000000UL // 1 second in nano seconds

static int gpo = GPIO_PIN_OUTPUT;
module_param(gpo, int, S_IRUGO);
MODULE_PARM_DESC(gpo, "GPIO output number");

static unsigned long ksec = PERIOD_SEC;
module_param(ksec, ulong, S_IRUGO);
MODULE_PARM_DESC(ksec, "Timer period seconds part of `ktime_set(secs, nanosecs)`");

static unsigned long knsec = PERIOD_NSEC;
module_param(knsec, ulong, S_IRUGO);
MODULE_PARM_DESC(knsec, "Timer period nano-seconds part of `ktime_set(secs, nanosecs)`");

static bool verbose = false;
module_param(verbose, bool, S_IRUGO);
MODULE_PARM_DESC(verbose, "(optional) verbose mode");

static struct hrtimer hr_timer;
static long unsigned int timer_count = 0;
static int state = 0;
 
static int __init mod_init(void);
static void __exit mod_exit(void);

// Timer Callback function. This will be called when timer expires
enum hrtimer_restart timer_callback(struct hrtimer *timer) {
	timer_count++;
	if (verbose) {
		printk(DRIVER_NAME ": timer callback count=%lu\n", timer_count);
	}

	gpio_set_value(gpo, state);
	state ^= 1;

	hrtimer_forward_now(timer, ktime_set(ksec, knsec));
	return HRTIMER_RESTART;
}

static void print_settings(void) {
	printk(DRIVER_NAME ": gpo=%d period={sec=%lu nsec=%lu}\n", gpo, ksec, knsec);
}

/* At load (insmod) */
static int __init mod_init(void) {
	printk(DRIVER_NAME ": mod_init\n");

	/* Set `gpo` to output mode. */
	gpio_direction_output(gpo, 1);

	/* Set `gpo` pin-out to 0(Low). */
	gpio_set_value(gpo, 0);

	print_settings();

	/* Setup the timer */
	ktime_t ktime;
	ktime = ktime_set(ksec, knsec);
	hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hr_timer.function = &timer_callback;
	hrtimer_start( &hr_timer, ktime, HRTIMER_MODE_REL);

	printk(DRIVER_NAME ": insmod done.\n");
	return 0;
}

/* At unload (rmmod) */
static void __exit mod_exit(void) {
	printk(DRIVER_NAME ": mod_exit\n");

	/* Stop the timer */
	hrtimer_cancel(&hr_timer);
}

module_init(mod_init);
module_exit(mod_exit);
