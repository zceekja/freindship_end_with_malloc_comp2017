CC=gcc
CFLAGS=-fsanitize=address -Wall -Werror -std=gnu11 -g -lm

tests: tests.c virtual_alloc.c  virtual_helper.c
	$(CC) $(CFLAGS) $^ -o $@

run_tests:
	./test_maker.sh
	./test.sh

clean:
	rm tests/test_binary/*
	