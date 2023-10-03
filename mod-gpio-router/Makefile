# KERNEL_HEADERS=/lib/modules/$(shell uname -r)/build
KERNEL_HEADERS=/usr/src/linux-headers-$(shell uname -r)

CFILES = driver.c
obj-m := gpio-router.o
gpio-router-objs := $(CFILES:.c=.o)

ccflags-y += -std=gnu99 -Wall -Wno-declaration-after-statement

all:
	make -C $(KERNEL_HEADERS) M=$(PWD) modules
clean:
	make -C $(KERNEL_HEADERS) M=$(PWD) clean

