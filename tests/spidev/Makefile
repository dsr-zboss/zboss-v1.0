all: spidev_test

clean:
	rm -f spidev_test spidev_test.o

rebuild: clean all

LINUX_HDRS=/home/zheka/zbarm/linux-2.6.31/include
CC=arm-none-linux-gnueabi-gcc
CFLAGS += -g -I$(LINUX_HDRS) -W -Wall

spidev_test: spidev_test.c