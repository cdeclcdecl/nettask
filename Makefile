all: 2ip

2ip: main.o server.o client.o
	gcc main.o server.o client.o -o 2ip

main.o: main.c 
	gcc -c main.c -o main.o

server.o: server.c 
	gcc -c server.c -o server.o

client.o: client.c
	gcc -c client.c -o client.o

clean:
	rm -rf *.o 2ip