
all: libopenat.so 
CFLAGS += -Wall -D__DEBUG__
FPIC ?= -fPIC
%.o: %.c
	$(CC) $(CFLAGS) $(FPIC) -c -o $@ $<

libopenat.so:openat.o
	$(CC) $(LDFLAGS) -shared -o $@ $<

clean:
	rm -f libopenat.so openat.o 
