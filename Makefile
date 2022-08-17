CC 		= 	gcc
FLAGS 	= 	-Wall -Werror -ggdb -lws2_32
SERVER	=	.\server.c
CLIENT	=	.\client.c

c-s:
	$(CC) $(SERVER) -o .\server.exe $(FLAGS)
	$(CC) $(CLIENT) -o .\client.exe $(FLAGS)