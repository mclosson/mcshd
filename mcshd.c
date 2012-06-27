#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <string.h>
#include <syslog.h>

#include "wrappedsockets.h"

#define MAXLEN 1024
#define DELAY 2
#define TTL 5

int main(int argc, char *argv[])
{
	int receivingSocket, sendingSocket;
	u_char ttl;
	struct sockaddr_in mcast_group;
	struct utsname name;

	if ((argc < 3) || (argc > 4)) {
		fprintf(stderr, "Usage: %s mcast_group port [ttl]\n", argv[0]);
		exit(1);
	}

	memset(&mcast_group, 0, sizeof(mcast_group));
	mcast_group.sin_family = AF_INET;
	mcast_group.sin_port = htons((unsigned short int)strtol(argv[2], NULL, 0));
	mcast_group.sin_addr.s_addr = inet_addr(argv[1]);

	allocateSocket(&sendingSocket);
	ttl = (argc == 4) ? strtol(argv[3], NULL, 0) : TTL;
	allocateSocket(&receivingSocket);
	enableSocketReuse(&receivingSocket);
	enableMulticastLoop(&receivingSocket);
	bindSocket(&receivingSocket, mcast_group);
	joinMulticastGroup(mcast_group.sin_addr, &receivingSocket);
	getSystemNodeName(&name);

	char buffer[MAXLEN + 1];
	FILE *fp;

	if (fork() == 0) {
		setsid();
		chdir("/");
		uname(0);
		openlog("mcshd", LOG_CONS, LOG_USER);

		for (;;) {
			memset(buffer, 0, sizeof(buffer));
			recvfrom(receivingSocket, buffer, MAXLEN, 0, NULL, NULL);
			
			if (strstr(buffer, "STARTOFCOMMAND: ") != NULL) {
				syslog(LOG_INFO, "Received command: %s", buffer);

				fp = popen(buffer+16, "r");
				
				if (fp != NULL) {
					while (fgets(buffer, MAXLEN, fp) != NULL) {
						sendto(sendingSocket, buffer, strlen(buffer)+1, 0, (const struct sockaddr *)&mcast_group, sizeof(mcast_group));
					}
					
					snprintf(buffer, MAXLEN, "ENDOFCOMMAND");
					sendto(sendingSocket, buffer, strlen(buffer)+1, 0, (const struct sockaddr *)&mcast_group, sizeof(mcast_group));
					pclose(fp);
				}
			}
		}
	}
	exit(0);
}
