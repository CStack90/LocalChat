#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <netdb.h>
#include <string.h>
#define PORT 2058
int main() { // serveur
int sock, nsock, lg, n;
pid_t pid;
char buf[20];
struct sockaddr_in adr_s, adr_c;
sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Creation socket
bzero(&adr_s,sizeof(adr_s)); adr_s.sin_family = AF_INET; adr_s.sin_port = htons(PORT);
adr_s.sin_addr.s_addr = htonl(INADDR_ANY);
bind(sock, (struct sockaddr *) &adr_s, sizeof(adr_s)); listen(sock, 5);
while (1)
{ lg = sizeof(adr_c);
nsock = accept(sock, (struct sockaddr *) &adr_c, &lg);
pid = fork();
if (pid==0)
{close(sock);
       	read(nsock,buf,20);
       	close(nsock);
       	printf("Message reçu : %s \n", buf);
exit(0);}
}
}
