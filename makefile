all: main.o client.o forking_server.o
	gcc -o ftp main.o client.o forking_server.o -lcrypt
main.o: main.c networking.h
	gcc -c main.c
client.o: client.c networking.h
	gcc -c client.c
forking_server.o: forking_server.c networking.h
	gcc -c forking_server.c
clean:
	rm *.o
	rm ftp
run: all
	./ftp
