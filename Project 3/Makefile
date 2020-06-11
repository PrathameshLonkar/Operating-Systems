ifeq ($(KERNELRELEASE),)
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

modules:
	make -C $(KERNELDIR) M=$(PWD) modules
	gcc user.c -o user
	
modules_install:
	sudo make -C $(KERNELDIR) M=$(PWD) modules_install

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c *.mod modules.order Module.symvers .tmp_version
else
	obj-m := kprobe_new.o
endif
