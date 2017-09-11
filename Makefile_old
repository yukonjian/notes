ARCH := arm64
CROSS_COMPILE := /home/root/cqa64_android_v6.0/lichee/out/sun50iw1p1/android/common/buildroot/external-toolchain/bin/aarch64-linux-gnu-
KERN_DIR = /home/root/cqa64_android_v6.0/lichee/linux-3.10

all:
	make -C $(KERN_DIR) M=`pwd` ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)  modules 

clean:
	make -C $(KERN_DIR) M=`pwd` ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules clean


obj-m	+= sunxi_led.o  sunxi_ledtest.o
sunxi_ledtest-objs := sunxi_led.o

