all: main.o client.o server.o
	gcc -o ftp main.o client.o server.o
main.o: main.c
	gcc -c main.c
client.o: client.c client.h
	gcc -c client.c
server.o: server.c server.h
	gcc -c server.c
clean:
	rm *.o
	rm ftp
run: all
	./ftp