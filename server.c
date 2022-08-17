/*
 *  Author: Vinicius Mari Marrafon
 *  Language: C
 *  Operating System: Windows NT 10.0
 */
// Server
// Obs: Funciona sem net!
// Mas sera que funciona com outros dispositivos?!
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define CONN_SUCCESS_MSG    "Welcome to File Transfer Server!"
#define FILE_MAXLEN 1 * 1048576 // 1 M

typedef struct FTP
{
    char path[100];
    int len;
    unsigned char file[FILE_MAXLEN];
}
FTP;

int main(int argc, char const *argv[])
{
    // Check for right usage of program
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s {Server_Port}\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Configure information about process that allow it to use sockets
    WSADATA ws;
    WSAStartup(0x0202, &ws);

    // Create Socket (family IPv4, TCP socket, TCP protocol)
    SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == INVALID_SOCKET)
    {
        fprintf(stderr, "Some wrong happen in socket creation!\n");
        exit(EXIT_FAILURE);
    }

    // Configure information about listening
    struct sockaddr_in s;
    
    ZeroMemory(&s, sizeof s);

    s.sin_family = AF_INET;                 // IPv4 
    s.sin_port = htons(atoi(argv[1]));      // Port
    s.sin_addr.s_addr = htonl(INADDR_ANY);  // Accept any ip address inside the Local Area Network (LAN)
    
    // Bind socket for everyone
    if (bind(server, (struct sockaddr *)& s, sizeof s) != SOCKET_ERROR)
    {
        // Listen
        listen(server, 1); // Listen for just one person
        
        // Print on terminal that the server is running and tell where exactly is the port
        printf("Server is running on port: %s\n", argv[1]);
    
        // Take the peek of queue connection
        SOCKET client = accept(server, NULL, NULL);
        printf("Client %d connected!\n", client);

        // Send message to the client
        send(client, CONN_SUCCESS_MSG, sizeof CONN_SUCCESS_MSG, 0);

        // char f_name[100] = {'\0'}; // Name with extension! of the client's file
		// char f_data[FILE_MAXLEN] = {'\0'}; // 100 K file

        _Bool running = 1;
        
		// Start to receive file through the connection
        while (running)
        {
            FTP package;
            // Receive the name, extension and data of the file
            recv(client, (void *)& package, sizeof package, 0);
            
            // Nao eh uma boa pratica => Bolha no pipeline
            // if (strcmp(buffer, "quit") == 0) break;

            // puts("Package Received:");
            // printf("Name:\t\t%s\n", package.path);
            // printf("len:\t\t%d\n", package.len);
            // puts("FILE:");
            // for (int i = 0; i < package.len; i++)
            //     printf("[%u]", (unsigned char)package.file[i]);

            // If the client want to logout!
            if (strcmp(package.path, "quit") == 0)
            {
                puts("Finishing connection! Take care!");
                running = 0;
            }

            // Otherwise, download the file to the "Base" directory
            else
            {
                char f_dir_store[] = "./Base/"; // Where the file will be stored
				// Concatenate the directory with the name of file
				strcat(f_dir_store, package.path);

                // Open/Create the file
				FILE *fptr = fopen(f_dir_store, "w");

				fwrite(package.file, package.len, 1, fptr);

                // printf("File stored in %s\n", f_dir_store);

                // printf("Data received: %s.\n", buffer);
				fclose(fptr);

                // Notify the client that the file has been received!
                send(client, "File Received!",  sizeof "File Received!", 0);
            }
        }
    }
    else
    {
        printf("Error: %d\n", WSAGetLastError());
    }
    
    WSACleanup();
    closesocket(server);

    return 0;
}
