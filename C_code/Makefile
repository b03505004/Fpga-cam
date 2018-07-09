#arm-linux-gnueabihf-gcc I2C_core.c -static -g -Wall  -I ${SOCEDS_DEST_ROOT}/ip/altera/hps/altera_hps/hwlib/include -o I2C_core.o -c
#arm-linux-gnueabihf-gcc main.o -g -Wall -o d8m

CC = arm-linux-gnueabihf-gcc
#CFLAGS = -static -g -Wall  -I ${SOCEDS_DEST_ROOT}/ip/altera/hps/altera_hps/hwlib/include
#LDFLAGS =  -g -Wall
CFLAGS = -static -g   -I ${SOCEDS_DEST_ROOT}/ip/altera/hps/altera_hps/hwlib/include
LDFLAGS =  -g 

d8m_new: io.o main.o I2C_core.o auto_focus.o mipi_camera_config.o mipi_bridge_config.o alt_errno.o altera_msgdma.o alt_find_dev.o alt_dev_llist_insert.o
	$(CC) $(LDFLAGS) io.o main.o I2C_core.o auto_focus.o mipi_camera_config.o mipi_bridge_config.o altera_msgdma.o alt_errno.o alt_find_dev.o alt_dev_llist_insert.o -o d8m_new

main.o: io.o I2C_core.o auto_focus.o mipi_camera_config.o mipi_bridge_config.o alt_errno.o altera_msgdma.o alt_find_dev.o alt_dev_llist_insert.o main.c
	$(CC) io.o I2C_core.o auto_focus.o mipi_camera_config.o mipi_bridge_config.o alt_errno.o altera_msgdma.o alt_find_dev.o alt_dev_llist_insert.o main.c  $(CFLAGS) -o main.o -c

altera_msgdma.o: alt_errno.o alt_find_dev.o alt_dev_llist_insert.o altera_msgdma.c
	$(CC) alt_errno.o alt_find_dev.o alt_dev_llist_insert.o altera_msgdma.c $(CFLAGS) -o altera_msgdma.o -c

alt_find_dev.o: alt_errno.o alt_find_dev.c
	$(CC) alt_errno.o alt_find_dev.c $(CFLAGS) -o alt_find_dev.o -c

#alt_iic.o: alt_iic_isr_register.o alt_iic.c
#	$(CC) alt_iic_isr_register.o alt_iic.c $(CFLAGS) -o alt_iic.o -c

#alt_iic_isr_register.o: alt_iic_isr_register.c
#	$(CC) alt_iic_isr_register.c $(CFLAGS) -o alt_iic_isr_register.o -c


alt_errno.o: alt_errno.c
	$(CC) alt_errno.c $(CFLAGS) -o alt_errno.o -c

alt_dev_llist_insert.o: alt_dev_llist_insert.c
	$(CC) alt_dev_llist_insert.c $(CFLAGS) -o alt_dev_llist_insert.o -c

mipi_camera_config.o: io.o I2C_core.o auto_focus.o mipi_camera_config.c 
	$(CC) io.o I2C_core.o auto_focus.o mipi_camera_config.c $(CFLAGS) -o mipi_camera_config.o -c

mipi_bridge_config.o: io.o I2C_core.o mipi_bridge_config.c 
	$(CC) io.o I2C_core.o mipi_bridge_config.c $(CFLAGS) -o mipi_bridge_config.o -c

auto_focus.o: io.o I2C_core.o auto_focus.c
	$(CC) io.o I2C_core.o auto_focus.c $(CFLAGS) -o auto_focus.o -c

I2C_core.o: io.o I2C_core.c 
	$(CC) io.o I2C_core.c $(CFLAGS) -o I2C_core.o -c
io.o: io.c
	$(CC) io.c $(CFLAGS) -o io.o -c
.PHONY: clean
clean:
	rm -f d8m_new *.a *.o

