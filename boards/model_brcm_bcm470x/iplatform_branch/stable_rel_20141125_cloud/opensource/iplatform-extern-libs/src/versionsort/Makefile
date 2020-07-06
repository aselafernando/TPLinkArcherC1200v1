
CFLAGS += -Wall -D__DEBUG__
FPIC ?= -fPIC

all: libversionsort.so 

%.o: %.c
	$(CC) $(CFLAGS) $(FPIC) -c -o $@ $<

libversionsort.so:versionsort.o
	$(CC) $(LDFLAGS) -shared -o $@ $<

clean:
	rm -f libversionsort.so versionsort.o 
