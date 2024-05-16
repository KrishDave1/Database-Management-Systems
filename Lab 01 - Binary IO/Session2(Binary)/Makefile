CC = gcc

DRIVERS = driver_session2a driver_session2b driver_session2c driver_session2d

all: $(DRIVERS)

$(DRIVERS) : % : %.c IMT2022043_lab2.c
	$(CC) -o $@ $^

clean:
	rm $(DRIVERS) 2a.bin 2c.bin

write_a:
	./driver_session2a 2a.bin
read_a:
	./driver_session2b 2a.bin
write_c:
	./driver_session2c 2c.bin
read_c:
	./driver_session2d 2c.bin