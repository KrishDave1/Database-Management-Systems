CC = gcc

DRIVERS = driver_session1a driver_session1b driver_session1c driver_session1d

all: $(DRIVERS)

$(DRIVERS) : % : %.c IMT2022043_lab1.c
	$(CC) -o $@ $^

clean:
	rm $(DRIVERS) 1a.txt 1c.txt

write_a:
	./driver_session1a 1a.txt
read_a:
	./driver_session1b 1a.txt
write_c:
	./driver_session1c 1c.txt
read_c:
	./driver_session1d 1c.txt