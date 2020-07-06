all: libfdopendir.so
CFLAGS=$(CFLAGS) -Wall -D__DEBUG__
FPIC ?= -fPIC
%.o: %.c
	$(CC) $(CFLAGS) $(FPIC) -c -o $@ $<

libfdopendir.so:fdopendir.o
	$(CC) $(LDFLAGS) -shared -o $@ $<

clean:
	rm -f libfdopendir.so fdopendir.o
