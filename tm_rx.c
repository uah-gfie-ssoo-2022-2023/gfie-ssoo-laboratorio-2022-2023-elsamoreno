#include <stdio.h> 
#include <netdb.h> 
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 

#define PORT 2500 

typedef enum {

    IN_SYNC_HEADER = 0,
    IN_RAW_SIZE = 1,
    IN_TM_DATA = 3

} state_t;

#define DEBUG(x...) printf(x)
  
void rx_tm(int sockfd) 
{ 
    uint8_t read_byte;

    uint8_t sync_header[4] = { 0xBE, 0xBA, 0xBE, 0xEF };
    uint8_t raw_size[2];
    uint16_t tm_packet_size = 0;
    
    uint8_t tm_bytes[256];

    state_t state = IN_SYNC_HEADER;
    uint8_t substate = 0;
    
    // infinite loop
    for (;;)
    { 
        // read a byte
        if (read(sockfd, &read_byte, 1) != 1) { 
        	return;
        } 

        switch (state) {

            case IN_SYNC_HEADER:
            if (read_byte != sync_header[substate]) {
                DEBUG("Unexpected byte: %d, expected: %d\n", read_byte, sync_header[substate]);
                DEBUG("Resetting frame\n");
                substate = 0;
            } else {
                // Advance to the next state
                substate = substate + 1;
                if (substate == 4) {
                    // We have already received the synchronization header
                    state = IN_RAW_SIZE;
                    substate = 0;
                }
            }
            break;
            case IN_RAW_SIZE:
            raw_size[substate] = read_byte;
            substate = substate + 1;
            if (substate == 2) {
                // We have already read the size
                tm_packet_size = raw_size[0];
                tm_packet_size = tm_packet_size << 8;
                tm_packet_size = tm_packet_size | raw_size[1];
                if (tm_packet_size >= 256) {
                    DEBUG("Unsupported packet size: %d\n", tm_packet_size);
                    DEBUG("Resetting frame\n");
                    state = IN_SYNC_HEADER;
                    substate = 0;
                    tm_packet_size = 0;
                } else {
                    state = IN_TM_DATA;
                    substate = 0;
                }
            }
            break;
            case IN_TM_DATA:
            tm_bytes[substate] = read_byte;
            substate = substate + 1;
            if (substate == tm_packet_size) {
                // We have read the whole TM packet
                DEBUG("Read TM:");
                for (uint16_t i = 0; i < tm_packet_size; i = i + 1) {
                    if (i % 16 == 0) {
                        DEBUG("\n0x%03X: ", i);
                    }
                    DEBUG("%02X ", tm_bytes[i]);
                }
                DEBUG("\nResetting frame\n");
                state = IN_SYNC_HEADER;
                substate = 0;
                tm_packet_size = 0;
            }
        }
     } 
} 
  
// Driver function 
int main() 
{ 
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse the address
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Set address and port for socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d...\n", PORT);

    // Accept incoming connections and receive data
    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))) {

        printf(">> New connection established.\n");

        rx_tm(new_socket);

        close(new_socket);

        printf("<< Connection lost.\n");
        printf("------------------------------------------------------\n");

    }

} 

