#
# Copyright (c) 2014 Shenzhen TP-LINK Technologies Co.Ltd.
#
# Author  : Ye Qianchuan <yeqianchuan@tp-link.net>
# Version : 1.0
# Date    : 9 Apr, 2014
#

RP_VERSION ?= 3.8p
CFLAGS += '-DRP_VERSION="$(RP_VERSION)"'

BIN := pppoe-discovery
SRC_C := $(wildcard *.c)
SRC_H := $(wildcard *.h)
OBJS := $(patsubst %.c,%.o,$(SRC_C))

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJS): %.o: %.c $(SRC_H)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(BIN) $(OBJS)
