// Service ppp - on port 3000
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define FILE_MAXLEN 1 * 1048576 // 1 M

void getFileSize(FILE *, size_t *);

typedef struct FTP
{
    char path[100];
    int len;
    unsigned char file[FILE_MAXLEN];
}
FTP;

int main(int argc, char const* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s {Server_Port}\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    WSADATA ws;
	WSAStartup(0x0202, &ws);

    SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in server;

    ZeroMemory(&server, sizeof server);
  
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the socket (what does this return?)
    int connectStatus = connect(client, (struct sockaddr *)& server, sizeof server);
    printf("connection status: %d\n", connectStatus);

    // CHeck if the socket is bind!
    if (connectStatus != SOCKET_ERROR)
    {
        // Data that will be received
        char connectionMsg[255];
		_Bool running = 1;

        // Welcome
        // recv return the number in blobs of message received!
        recv(client, connectionMsg, sizeof connectionMsg, 0);
        puts(connectionMsg);

        while (running)
        {
			// Variables
			char f_name[100] = {'\0'};

			// Read the information about file]
			printf("Type the name of file with extension and path of file to transfer: ");
			fgets(f_name, 99, stdin);
			f_name[strcspn(f_name, "\r\n")] = '\0';
		
			if (strcmp(f_name, "quit") == 0)
			{
				send(client, "quit", sizeof "quit", 0);
				send(client, NULL, 0, 0);

				running = 0;
			}	
			else
			{
                FTP package;
                size_t file_size = 0;

                // Copy file
                FILE *fptr = fopen(f_name, "r");
                if (fptr == NULL)
                {
                    fprintf(stderr, "Couldn't opne file!");
                    continue;
                }

                getFileSize(fptr, &file_size);

                rewind(fptr);

                // puts("Reading file...");
                fread(package.file, file_size, 1, fptr);

                fclose(fptr);

                // End file

                // puts("Taking the name of file!");
				// Send only the name of file to the server

                const char delim[] = "/";
                char *token;
                char name[100] = {'\0'};

                // Take only the name of the file!
                token = strtok(f_name, delim);
                while (token != NULL)
                {
                    strcpy(name, token);
                    token = strtok(NULL, delim);
                }

                strcpy(package.path, name);

                package.len = file_size;

                // Send package to the server
                send(client, (void *)& package, sizeof package, 0);

                // Wait for notification
                char ans[100] = {'\0'};
                recv(client, ans, sizeof ans, 0);
                printf("Server: %s\n", ans);
			}
        }
    }
    else
    {
        // Error: 10060 => Connection Time Out
        printf("Can't connect! Error: %d\n", WSAGetLastError());
    }
    
    closesocket(client);
    WSACleanup();
    
    return 0;
}

// OK!
void getFileSize(FILE *file, size_t *file_size)
{
    unsigned char c[2] = {'\0'};
    while (fread(c, 1, 1, file))
        (* file_size)++;
}