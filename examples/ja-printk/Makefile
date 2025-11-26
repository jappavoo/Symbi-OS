CC = gcc
CFLAGS = -O0 -ggdb -Wall -Wextra -mno-red-zone -fno-omit-frame-pointer -Wfatal-errors \
	-fPIC
CFLAGS += -I ../common -I ../../Symlib/include -DDYNLINK

TARGETS :=  main main_static 
all: $(TARGETS)

# kernel module build
ext.kbin: $(wildcard *.kc *.kh)
	../tools/kcc

# main program
main.o: main.c ../common/dpld.h
	$(CC) $(CFLAGS) -c -o main.o main.c

main: ext.kbin main.o ../common/dpld.o
	$(CC) $(CFLAGS) -o $@ $^ -lc -L../../Symlib/dynam_build -lSym -L/proc -lkallsyms 

main_static: ext.kbin ../common/dpld.o main.o 
	$(CC) $(CFLAGS) -o $@ $^ /proc/libkallsyms.a -lc -L../../Symlib/dynam_build -lSym 

clean:
	rm -rf $(wildcard $(TARGETS) *.o *.so *.a *.kbin .ext)

.PHONY: all clean
