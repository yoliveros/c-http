server: server.c
	$(CC) server.c -o server -Wall -Wextra -pedantic -std=c17

dev: server.c
	$(CC) server.c -o server.o 
