all: main.o client.o forking_server.o
	gcc -o ftp main.o client.o
main.o: main.c client.h
	gcc -c main.c
client.o: client.c networking.h client.h
	gcc -c client.c
forking_server.o: forking_server.c networking.h
	gcc -o forking_server forking_server.c
clean:
	rm *.o
	rm ftp
run: all
	./ftp