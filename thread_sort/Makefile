obj-m := sort_compare.o thread_sort.o thread_sort_depth.o

KERNEL_DIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default : 
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules
clean : 
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD) clean
