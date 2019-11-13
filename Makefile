CFLAGS=-std=c11 -g -static -fno-common
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

test: 9cc
	./test.sh

clean:
	rm -f 9cc *.o *~ tmp*

docker/build:
	docker build -t compilebook .

docker/run:
	docker run --rm -it -v $(HOME)/9cc:/9cc compilebook

.PHONY: test clean docker
