CC = g++

LIB = -pthread

all: lite-server

TARGET = lite-server

lite-server: server.cpp locker.h thread_pool.h task.h
	$(CC) -o lite-server server.cpp locker.h thread_pool.h task.h $(LIB)

clean:
		rm -f *.o *~
