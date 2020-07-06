
all: libunshare.so 
CFLAGS += -Wall -D__DEBUG__
FPIC ?= -fPIC
%.o: %.c
	$(CC) $(CFLAGS) $(FPIC) -c -o $@ $<

libunshare.so:unshare.o
	$(CC) $(LDFLAGS) -shared -o $@ $<

clean:
	rm -f libunshare.so unshare.o 
