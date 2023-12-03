#
# Tony Givargis
# Copyright (C), 2023
# University of California, Irvine
#
# CS 238P - Operating Systems
# Makefile -ansi -pedantic -Wall -Wextra -Werror -Wfatal-errors
#

CC     = gcc -Wformat=0 -g
# CC     = gcc -Wformat=0 -g -DDBG_TO_STDOUT
CFLAGS =  -fpic -O3
LDLIBS = -lpthread
DEST   = cs238
SRCS  := $(wildcard *.c)
OBJS  := $(SRCS:.c=.o)

all: $(OBJS)
	@echo "[LN]" $(DEST)
	@$(CC) -o $(DEST) $(OBJS) $(LDLIBS)

%.o: %.c
	@echo "[CC]" $<
	@$(CC) $(CFLAGS) -c $<
	@$(CC) $(CFLAGS) -MM $< > $*.d

clean:
	@rm -f $(DEST) *.so *.o *.d *~ *#

-include $(OBJS:.o=.d)
