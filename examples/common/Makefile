CFLAGS=CC = gcc
CFLAGS = -O0 -ggdb -Wall -Wextra -mno-red-zone -fno-omit-frame-pointer -Wfatal-errors \
	-fPIC
CFLAGS += -I ../../Symlib/include -DDYNLINK
TARGETS := dpld.o
.PHONY: all clean

all: $(TARGETS)

# dynamic priveleged runtime loader
dpld.o: dpld.c dpld.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-${RM} -rf $(wildcard $(TARGETS) *.o)
