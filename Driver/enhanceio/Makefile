# test
# Makefile for EnhanceIO block device caching.
#

COMMIT_REV ?= $(shell echo)
KERNEL_SOURCE_VERSION ?= $(shell uname -r)
KERNEL_TREE ?= /lib/modules/$(KERNEL_SOURCE_VERSION)/build
EXTRA_CFLAGS += -I$(KERNEL_TREE)/drivers/md -I./ -DCOMMIT_REV="\"$(COMMIT_REV)\""
EXTRA_CFLAGS += -I$(KERNEL_TREE)/include/ -I$(KERNEL_TREE)/include/linux 
obj-m	+= enhanceio.o enhanceio_lru.o enhanceio_fifo.o  enhanceio_rand.o
enhanceio-y	+= \
	eio_conf.o \
	eio_ioctl.o \
	eio_main.o \
	eio_mem.o \
	eio_policy.o \
	eio_procfs.o \
	eio_setlru.o \
	eio_subr.o \
	eio_ttc.o
enhanceio_fifo-y	+= eio_fifo.o
enhanceio_rand-y	+= eio_rand.o
enhanceio_lru-y	+= eio_lru.o
.PHONY: all
all: modules 
.PHONY:    modules
modules:
	make -C $(KERNEL_TREE) M=$(PWD) modules V=0
.PHONY: modules_install
modules_install: modules
	install -o root -g root -m 0755 -d $(DESTDIR)/lib/modules/$(KERNEL_SOURCE_VERSION)/extra/enhanceio/
	install -o root -g root -m 0755 enhanceio.ko $(DESTDIR)/lib/modules/$(KERNEL_SOURCE_VERSION)/extra/enhanceio/
	install -o root -g root -m 0755 enhanceio_rand.ko $(DESTDIR)/lib/modules/$(KERNEL_SOURCE_VERSION)/extra/enhanceio/
	install -o root -g root -m 0755 enhanceio_fifo.ko $(DESTDIR)/lib/modules/$(KERNEL_SOURCE_VERSION)/extra/enhanceio/
	install -o root -g root -m 0755 enhanceio_lru.ko $(DESTDIR)/lib/modules/$(KERNEL_SOURCE_VERSION)/extra/enhanceio/
	depmod -a
.PHONY: install
install: modules_install
.PHONY: clean
clean:
	make -C $(KERNEL_TREE) M=$(PWD) clean
