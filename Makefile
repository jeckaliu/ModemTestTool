#设置编译环境
CC=gcc
all: modemtest
modemtest: modemMain.c
				$(CC)  -o $@ modemMain.c  modemPortHandle.c atCmdHandle.c 

clean:
				rm modemtest

.PHONY: all   clean   