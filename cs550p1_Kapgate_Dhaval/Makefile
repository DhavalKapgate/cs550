CC=gcc
CXX=g++
#CC=clang
#CXX=clang++
DEBUG=\
 -DCS550_ASSERT \
 -DCS550_DBG_PRINT \

.PHONY: a.out
a.out:
	$(CC) -g $(DEBUG) -Wall -Wextra test.c malloc.c utility.c
