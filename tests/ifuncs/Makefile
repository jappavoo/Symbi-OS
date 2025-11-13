CC = gcc
CFLAGS = -O0 -ggdb -Wall -Wextra -mno-red-zone -fno-omit-frame-pointer -Wfatal-errors \
	-fPIC
CFLAGS += -I ../../Symlib/include -DDYNLINK

all: $(TARGET)

mylib.o: mylib.c
	$(CC) $(CFLAGS) -c -o mylib.o mylib.c

libmylib.so: mylib.o mylib.vers
	$(CC) -shared -Wl,--version-script=mylib.vers -o libmylib.so mylib.o

# Stripped version of the library (no debug sections)
libmylib.nodebug.so: libmylib.so
	objcopy --strip-debug libmylib.so libmylib.nodebug.so

libmylib.stripall.so: libmylib.so
	objcopy --strip-all libmylib.so libmylib.stripall.so

# Version with no .symtab/.strtab (keeps .dynsym/.dynstr etc. for runtime)
libmylib.novers.so: libmylib.so
	objcopy \
		--remove-section=.gnu.version \
		--remove-section=.gnu.version_d \
		libmylib.so libmylib.novers.so

greeter.ko:
	mkdir -p ../../built_modules
	$(MAKE) -C ../.. build_module_full MODULE_SRC=./linux-kernel-module/greeter MODULE_DEST=/root/greeter MODULE_NAME=greeter V=1 FEDORA_RELEASE=43
	cp ../../built_modules/greeter.ko .

greeter.o: greeter.ko
	ld -r -b binary -o greeter.o greeter.ko

load_mod.o: load_mod.c
	$(CC) $(CFLAGS) -c -o load_mod.o load_mod.c

ifunc_elevate.o: ifunc_elevate.c
	$(CC) $(CFLAGS) -c -o ifunc_elevate.o ifunc_elevate.c

ifunc_elevate.preproc.c: ifunc_elevate.c
	$(CC) $(CFLAGS) -E ifunc_elevate.c -o ifunc_elevate.preproc.c

i_load_mod: greeter.o load_mod.o ifunc_elevate.o libmylib.so libkallsyms.so
	$(CC) $(CFLAGS) -o i_load_mod greeter.o load_mod.o ifunc_elevate.o -lc -L. -lmylib -L../../Symlib/dynam_build -lSym -L. -lkallsyms 
	readelf -Wa i_load_mod > i_load_mod.readelf.txt

libkallsyms.so:
	sudo cp /proc/libkallsyms.so .

libkallsyms.a:
	sudo cp /proc/libkallsyms.a .

i_load_mod_static: greeter.o load_mod.o ifunc_elevate.o libmylib.so libkallsyms.a
	$(CC) $(CFLAGS) -o i_load_mod_static greeter.o load_mod.o ifunc_elevate.o libkallsyms.a -lc -L. -lmylib -L../../Symlib/dynam_build -lSym 
	readelf -Wa i_load_mod_static > i_load_mod_static.readelf.txt


ifunc_e_early: CFLAGS +=  -L../../Symlib/dynam_build -lSym -I ../../Symlib/include
ifunc_e_early: ifunc_elevate.c
	$(CC) $(CFLAGS) -DE_EARLY -o ifunc_e_early ifunc_elevate.c
	

ifunc_elevate: CFLAGS +=  -L../../Symlib/dynam_build -lSym -I ../../Symlib/include
ifunc_elevate: ifunc_elevate.c
	$(CC) $(CFLAGS) -o ifunc_elevate ifunc_elevate.c


clean:
	rm -f ifunc_elevate ifunc_e_early i_load_mod i_load_mod_static greeter.o greeter.ko load_mod.o ifunc_elevate.o ifunc_elevate.preproc.c libmylib.so libmylib.nodebug.so libmylib.nosymtab.so mylib.o libkallsyms.* *.readelf.txt
	$(MAKE) -C ../.. clean_module MODULE_SRC=./linux-kernel-module/greeter MODULE_DEST=/root/greeter MODULE_NAME=greeter FEDORA_RELEASE=43

.PHONY: all clean run