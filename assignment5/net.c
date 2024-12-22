#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <err.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "net.h"
#include "jbod.h"

/* the client socket descriptor for the connection to the server */
int cli_sd = -1;

/* attempts to read n bytes from fd; returns true on success and false on
 * failure */
bool nread(int fd, int len, uint8_t *buf) {
  
  int read_bytes = read(fd, buf, len);
  if (read_bytes < 0) {
    return false;
  }
  return true;
}

/* attempts to write n bytes to fd; returns true on success and false on
 * failure */
bool nwrite(int fd, int len, uint8_t *buf) {

  int written_bytes = write(fd, buf, len);
  if (written_bytes < 0) {
      return false;
  }
  return true;
}

/* attempts to receive a packet from fd; returns true on success and false on
 * failure */
bool recv_packet(int fd, uint32_t *op, uint8_t *ret, uint8_t *block) {
  uint8_t header[HEADER_LEN];
  
  if(!nread(fd, HEADER_LEN, header)) {
    return false;
  }

  memcpy(op, header, sizeof(*op));
  *op = ntohl(*op);

  memcpy(ret, &header[sizeof(*op)], sizeof(*ret));

  if ((*ret & 0x02) != 0){ // block needed
    nread(fd, JBOD_BLOCK_SIZE, block);
  }
  return true;
}

/* attempts to send a packet to sd; returns true on success and false on
 * failure */
bool send_packet(int fd, uint32_t op, uint8_t *block) {
  uint8_t header[HEADER_LEN + JBOD_BLOCK_SIZE];
  op = htonl(op);
  memcpy(header, &op, sizeof(op));
  if (block != NULL) {
    header[sizeof(op)] = 0x02;
    
  } else {
    header[sizeof(op)] = 0x00;
  }

  if (!nwrite(fd, HEADER_LEN, header)) {
    return false;
  }

  // write
  if (block != NULL) {
    memcpy(&header[5], block, JBOD_BLOCK_SIZE);
    nwrite(fd, JBOD_BLOCK_SIZE, block);
  }

  return true;

}

/* connect to server and set the global client variable to the socket */
bool jbod_connect(const char *ip, uint16_t port) {
  struct sockaddr_in address;

  // Address info
  address.sin_family = AF_INET;
  address.sin_port = htons(port);

  if(inet_aton(ip, &address.sin_addr) == 0){
    return false;
  }

  // Creates a socket
  cli_sd = socket(AF_INET, SOCK_STREAM, 0);
  if (cli_sd == -1){
    printf("Error in socket creation [%s]\n", strerror(errno));
    return false;
  }

  // Connect
  if (connect(cli_sd, (const struct sockaddr *) &address, sizeof(address)) == -1){
    printf("Error in socket connection [%s]\n", strerror(errno));
    
    return false;
  }
  return true;

}

// Disconnect
void jbod_disconnect(void) {
  if (cli_sd != -1) {
    close(cli_sd);
    cli_sd = -1;
  }
}

// Client operation
int jbod_client_operation(uint32_t op, uint8_t *block) {

  // Checks to send packet
  if (!send_packet(cli_sd, op, block)) {
    return -1;
  }
  
  uint8_t ret;

  // Checks to receive packet
  if (!recv_packet(cli_sd, &op, &ret, block)) {
    return -1;
  }

  ret = ret & 0x01;

  // Compares ret to see if the operation succeeded
  if (ret == 0){
    return 0;
  }
  return -1;
  
}
