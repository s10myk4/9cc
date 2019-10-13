CFLAGS=c11 -g -static

9cc: 9cc.c

test: 9cc
	./test.sh

clean:
	rm -f9cc *.o *~ tmp*

docker/build:
	docker build -t compilebook .

docker/run:
	docker run --rm -it -v $(HOME)/9cc:/9cc compilebook

.PHONY: test clean docker