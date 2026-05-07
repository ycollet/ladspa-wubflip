all: wubflip.so

wubflip.so: wubflip.c
	gcc $(CFLAGS) -c -o wubflip.o wubflip.c
	gcc $(LDFLAGS) -shared -o wubflip.so wubflip.o

install:
	cp wubflip.so /usr/lib64/ladspa
	chmod 755 /usr/lib64/ladspa/wubflip.so

clean:
	rm *.o *.so
