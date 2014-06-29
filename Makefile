obj-m += nikitich.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	sh ./install.sh

uninstall:
	rmmod nikitich; rm /dev/nikitich
