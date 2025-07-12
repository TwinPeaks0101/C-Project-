#!/bin/sh
gcc -Wall -pthread p3220242-p3220189-p3120203-pizza.c -o p3220242-p3220189-p3120203-pizza

if [ -f p3220242-p3220189-p3120203-pizza ]; then
	./p3220242-p3220189-p3120203-pizza 100 10
else
	echo "Failed"
fi
