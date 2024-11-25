#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "protocol.h" // Header file defining DEFAULT_IP, DEFAULT_PORT, and BUFFER_SIZE

int main() {
    //Create a TCP socket
    int client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket < 0) {
        perror("Socket creation failed"); // Print an error message if socket creation fails
        exit(EXIT_FAILURE);              // Terminate the program
    }

    //Define the server's address and port
    struct sockaddr_in server_address = {0};   // Initialize the structure to zero
    server_address.sin_family = AF_INET;       // Use IPv4
    server_address.sin_addr.s_addr = inet_addr(DEFAULT_IP); // Server's IP address
    server_address.sin_port = htons(DEFAULT_PORT);          // Server's port number

    //Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");   // Print an error message if the connection fails
        close(client_socket);          // Close the socket
        exit(EXIT_FAILURE);            // Terminate the program
    }

    printf("Connected to the server.\n"); // Inform the user that the connection was successful

    char buffer[BUFFER_SIZE]; // Buffer for sending/receiving data

    //Interaction loop
    while (1) {
        //Display instructions for the user
        printf("Enter request (n=numeric, a=alphabetic, m=mixed, s=secure) EX: n 8\n");
        printf("Length must be between 6 and 32 characters.\n");
        printf("To quit, enter 'q'.\n");
        printf("Your request: ");

        // Read user input
        fgets(buffer, BUFFER_SIZE, stdin);

        // Check if the user wants to quit
        if (buffer[0] == 'q') {
            printf("Closing connection...\n"); // Inform the user that the connection is closing
            break;                            // Exit the loop
        }

        //Send the request to the server
        send(client_socket, buffer, strlen(buffer), 0);

        //Wait for the server's response
        int received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (received <= 0) {  // Check if the reception failed or the connection was closed
            perror("Receive failed"); // Print an error message
            break;                    // Exit the loop
        }

        // Null-terminate the received data and display it
        buffer[received] = '\0'; // Add a null character to mark the end of the string
        printf("Response from server: %s", buffer); // Server's response
    }

    //Close the socket
    close(client_socket);
    return 0;             // End the program
}
