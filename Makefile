CC = g++

LIB = -pthread

all: lite-server

TARGET = lite-server

lite-server: server.cpp locker.h thread_pool.h request_handler.h request_listener.h
	$(CC) -o lite-server server.cpp locker.h thread_pool.h request_handler.h request_listener.h $(LIB)

clean:
		rm -f *.o *~
