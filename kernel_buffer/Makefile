obj-m += kernel_buffer.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	$(CC) kbuff_application.c -o kbuff_app

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
