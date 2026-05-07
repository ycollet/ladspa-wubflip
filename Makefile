all:wubflip.so

wubflip.so: wubflip.c
	gcc -Wall -fPIC -dPIC -c -o wubflip.o wubflip.c
	ld -shared -o wubflip.so wubflip.o

install:
	cp wubflip.so /usr/lib/ladspa
	chmod 644 /usr/lib/ladspa/wubflip.so

clean:
	rm *.o *.so
