CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

rincc: $(OBJS)
	$(CC) -o rincc $(OBJS) $(LDFLAGS)

$(OBJS): rincc.h

test: rincc
	./test.sh

clean:
	rm -f rincc *.o *~ tmp*

.PHONY: test clean