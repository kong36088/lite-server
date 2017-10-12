CC = g++

LIB = -pthread

all: lite-server

TARGET = lite-server

lite-server: server.cpp locker.h thread_pool.h request_handler.hpp request_listener.hpp
	$(CC) -o lite-server server.cpp locker.h thread_pool.h request_handler.hpp request_listener.hpp $(LIB)

clean:
		rm -f *.o *~
