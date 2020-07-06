#
# mk for libifaddrs.so
#
CFLAGS += -Wall -D__DEBUG__
FPIC ?= -fPIC

all: ifaddrs.so

%.o: %.c
	$(CC) $(CFLAGS) $(FPIC) -c -o $@ $<

ifaddrs.so: ifaddrs.o
	$(CC) $(LDFLAGS) -shared -o libifaddrs.so ifaddrs.o

clean:
	rm -f ifaddrs.so ifaddrs.o
