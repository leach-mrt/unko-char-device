obj-m += src/unko.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

insmod:
	insmod src/unko.ko

rmmod:
	rmmod src/unko.ko

showdev:
	cat /proc/devices |grep unko

mknod:
	mknod -m 666 /dev/unko c ${MAJOR} ${MINOR}

rm:
	rm /dev/unko

cat:
	cat /dev/unko
