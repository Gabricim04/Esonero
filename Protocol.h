#ifndef PROTOCOL_H
#define PROTOCOL_H

#define DEFAULT_PORT 57015          // Default port for communication
#define DEFAULT_IP "127.0.0.1"      // Default IP address (localhost)
#define MAX_PASSWORD_LENGTH 32      // Max password length
#define MIN_PASSWORD_LENGTH 6       // Min password length
#define BUFFER_SIZE 256             // Buffer size for communication

// Function declarations for password generation
void generate_numeric(char *password, int length);
void generate_alpha(char *password, int length);
void generate_mixed(char *password, int length);
void generate_secure(char *password, int length);

#endif
