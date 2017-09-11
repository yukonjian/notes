
KERN_DIR = /usr/src/linux-headers-3.11.0-15-generic/

all:
	make -C $(KERN_DIR) M=`pwd`   modules 

clean:
	make -C $(KERN_DIR) M=`pwd`  modules clean


obj-m	+=  sunxi_ledtest.o
sunxi_ledtest-objs := sunxi_led.o

