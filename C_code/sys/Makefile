#arm-linux-gnueabihf-gcc I2C_core.c -static -g -Wall  -I ${SOCEDS_DEST_ROOT}/ip/altera/hps/altera_hps/hwlib/include -o I2C_core.o -c
#arm-linux-gnueabihf-gcc main.o -g -Wall -o d8m

CC = arm-linux-gnueabihf-gcc
#CFLAGS = -static -g -Wall  -I ${SOCEDS_DEST_ROOT}/ip/altera/hps/altera_hps/hwlib/include
#LDFLAGS =  -g -Wall
CFLAGS = -static -g   -I ${SOCEDS_DEST_ROOT}/ip/altera/hps/altera_hps/hwlib/include
LDFLAGS =  -g 

alt_errno.o: alt_errno.c
	$(CC) alt_errno.c  $(CFLAGS) -o alt_errno.o -c

.PHONY: clean
clean:
	rm -f d8m *.a *.o

