server:server.o
	gcc -I/usr/include/mysql server.o -L/usr/lib64/mysql -lmysqlclient -g -o server -lpthread
server.o:server.c sock_head.h
	gcc -I/usr/include/mysql server.c -L/usr/lib64/mysql -lmysqlclient -g -c -lpthread
clean:
	rm server server.o
