all: main.o client.o forking_server.o
	gcc -o ftp main.o client.o -lcrypt
main.o: main.c client.h
	gcc -c main.c
client.o: client.c networking.h client.h
	gcc -c client.c
clean:
	rm *.o
	rm ftp
run: all
	./ftp
