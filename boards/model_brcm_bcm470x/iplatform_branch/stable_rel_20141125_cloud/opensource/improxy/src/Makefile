MYCFLAGS +=  -Wall -I./include

SOURCES = $(wildcard ./src/*.c)
OBJS = $(patsubst %.c,%.o,$(SOURCES))

all : improxy

%.o: %.c
	$(CC) -MMD $(MYCFLAGS) -g -c -o $@ $< 

improxy : $(OBJS)
	$(CC) $(LDFLAGS) -o $@  $(OBJS) 

-include $(OBJS:.o=.d)

clean :
	rm -rf ${OBJS} $(DEPS) improxy
