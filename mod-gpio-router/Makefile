# KERNEL_HEADERS=/lib/modules/$(shell uname -r)/build
KERNEL_HEADERS=/usr/src/linux-headers-$(shell uname -r)

CFILES = driver.c
obj-m := gpio-router.o
gpio-router-objs := $(CFILES:.c=.o)

ccflags-y += -std=gnu99 -Wall -Wno-declaration-after-statement

all:
	$(MAKE) -C $(KERNEL_HEADERS) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNEL_HEADERS) M=$(PWD) clean

.PHONY: rmmod insmod
rmmod: all
	-sudo rmmod $(obj-m:.o=)
	-lsmod | grep $(obj-m:.o=)

insmod: rmmod
	-modprobe $(obj-m:.o=)
	#sudo insmod $(obj-m:.o=.ko) gpi=16 gpo=42 gpi_opt="l" reverse=true verbose=true
	sudo insmod $(obj-m:.o=.ko) gpi=16 gpo=42 gpi_opt="rf" reverse=true verbose=true
	grep "" -r /sys/module/gpio_router/parameters/

test:
	raspi-gpio set 16 ip pu
	raspi-gpio set 42 op pn
	raspi-gpio get 16,42
