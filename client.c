#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "wrappedsockets.h"

#define HELLO_PORT 5000
#define HELLO_GROUP "225.31.33.7"
#define MAXLEN 1024

int main(int argc, char *argv[])
{
  struct sockaddr_in addr;
  int sock, listener;
  char buffer[MAXLEN + 1];
  char command[MAXLEN + 1];

  if ((sock=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
    perror("socket");
    exit(1);
  }

  initializeIPAddress(&addr, HELLO_GROUP, HELLO_PORT);
  allocateSocket(&listener);
  enableSocketReuse(&listener);
  enableMulticastLoop(&listener);
  bindSocket(&listener, addr);
  joinMulticastGroup(addr.sin_addr, &listener);
    
  while (1) {
    memset(buffer, 0, sizeof(buffer));
    fprintf(stdout, "mc#: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strlen(buffer)-1] = '\0'; /* replace newline with null terminator, don't send the newline */
    strncpy(command, "STARTOFCOMMAND: ", MAXLEN);
    strncat(command, buffer, MAXLEN - 16);
    sendToSocket(&sock, command, addr);

    // fix this next line we want to know if the response is spoofed!!! 
    while (strcmp(buffer, "ENDOFCOMMAND")) {
      recvfrom(listener, buffer, sizeof(buffer), 0, NULL, NULL); 
      if (strcmp(buffer, "ENDOFCOMMAND") && strstr(buffer, "STARTOFCOMMAND: ") == NULL) {
        fprintf(stdout, "%s", buffer);
      }
    }
  }

  exit(0);
}
