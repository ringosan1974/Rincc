CFLAGS=-std=c11 -g -static

rincc: rincc.c

test: rincc
	./test.sh

clean:
	rm -f rincc *.o *~ tmp*

.PHONY: test clean