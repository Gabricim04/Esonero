#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "protocol.h" //Header file defining constants like DEFAULT_IP, DEFAULT_PORT, BUFFER_SIZE, etc.
#include <time.h>

//Function to generate numeric passwords (only digits)
void generate_numeric(char *password, int length) {
    for (int i = 0; i < length; i++) {
        password[i] = '0' + (rand() % 10); //Random digit from '0' to '9'
    }
    password[length] = '\0'; //Null-terminate the string
}

// Function to generate alphabetic passwords (only lowercase letters)
void generate_alpha(char *password, int length) {
    for (int i = 0; i < length; i++) {
        password[i] = 'a' + (rand() % 26); // Random letter from 'a' to 'z'
    }
    password[length] = '\0'; // Null-terminate the string
}

// Function to generate mixed passwords(lowercase letters and digits)
void generate_mixed(char *password, int length) {
    for (int i = 0; i < length; i++) {
        if (rand() % 2) { // 50% chance of picking a letter or a digit
            password[i] = 'a' + (rand() % 26); //Random letter
        } else {
            password[i] = '0' + (rand() % 10); //Random digit
        }
    }
    password[length] = '\0'; //Null-terminate the string
}

// Function to generate secure passwords (letters, digits, symbols)
void generate_secure(char *password, int length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
    int charset_size = sizeof(charset) - 1; //Size of charset excluding the null terminator

    for (int i = 0; i < length; i++) {
        password[i] = charset[rand() % charset_size]; //Random character
    }
    password[length] = '\0'; // Null-terminate the string
}

int main() {
    srand(time(NULL)); // Seed the random number generator with the current time

    //Create a TCP socket
    int server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket < 0) {
        perror("Socket creation failed"); // Print an error message if socket creation fails
        exit(EXIT_FAILURE);              // Terminate the program
    }

    //Configure the server address and port
    struct sockaddr_in server_address = {0};   // Initialize the structure to zero
    server_address.sin_family = AF_INET;       // Use IPv4
    server_address.sin_addr.s_addr = inet_addr(DEFAULT_IP); // Server's IP address
    server_address.sin_port = htons(DEFAULT_PORT);          // Server's port number

    //Bind the socket to the configured address and port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Bind failed"); //Print an error message if binding fails
        close(server_socket);  //Close the socket before exiting
        exit(EXIT_FAILURE);    //Terminate the program
    }

    //Listen for incoming client connections
    if (listen(server_socket, 5) < 0) { // Allow up to 5 pending client connections
        perror("Listen failed"); // Print an error message if listening fails
        close(server_socket);    // Close the socket before exiting
        exit(EXIT_FAILURE);      // Terminate the program
    }

    printf("Server is running on %s:%d\n", DEFAULT_IP, DEFAULT_PORT); //Inform the user that the server is ready

    // Main server loop to handle client connections
    while (1) {
        struct sockaddr_in client_address; //Structure to store the client's address
        socklen_t client_len = sizeof(client_address); //Size of the client's address structure
        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len); // Accept a new connection

        if (client_socket < 0) {
            perror("Accept failed"); //Print an error message if accepting a connection fails
            continue;                //Continue to the next iteration to accept new connections
        }

        // Display the client's IP address and port
        printf("New connection from %s:%d\n",
               inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        char buffer[BUFFER_SIZE]; //Buffer to store received data
        int received = recv(client_socket, buffer, BUFFER_SIZE, 0); //Receive data from the client
        if (received <= 0) {  //Check if reception failed or connection was closed
            perror("Receive failed"); //Print an error message
            close(client_socket);     //Close the client's socket
            continue;                 //Continue to the next iteration
        }

        buffer[received] = '\0'; //Null-terminate the received data
        char type;               //Type of password requested
        int length;              //Length of the password requested
        sscanf(buffer, "%c %d", &type, &length); //Read the type and length from the client message

        //Check if the requested length is within the allowed range
        if (length < MIN_PASSWORD_LENGTH || length > MAX_PASSWORD_LENGTH) {
            snprintf(buffer, BUFFER_SIZE, "Error: Length must be between %d and %d\n",
                     MIN_PASSWORD_LENGTH, MAX_PASSWORD_LENGTH);
        } else {
            char password[MAX_PASSWORD_LENGTH + 1]; //Buffer to store the generated password
            switch (type) { //Password type and generation
                case 'n':
                    generate_numeric(password, length); //Numeric password
                    break;
                case 'a':
                    generate_alpha(password, length);  //Alphabetic password
                    break;
                case 'm':
                    generate_mixed(password, length);  //Mixed password
                    break;
                case 's':
                    generate_secure(password, length); //Secure password
                    break;
                default:
                    snprintf(buffer, BUFFER_SIZE, "Error: Unknown password type\n"); // Invalid type
                    send(client_socket, buffer, strlen(buffer), 0); // Send error message
                    close(client_socket);                           // Close the client's socket
                    continue;                                       // Continue to the next iteration
            }
            snprintf(buffer, BUFFER_SIZE, "Password: %s\n", password); // Format the generated password
        }

        send(client_socket, buffer, strlen(buffer), 0); //Send the response to the client
        close(client_socket);                           //Close the client's socket
    }

    close(server_socket); // Close the server's socket
    return 0;             // End the program
}

