#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#define PORT 2058
int main() { // serveur
int sock, lg, n;
char buf[20];
struct sockaddr_in adr_s, adr_c;
sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // Creation socket
bzero(&adr_s,sizeof(adr_s)); 
adr_s.sin_family = AF_INET;
adr_s.sin_port = htons(PORT);
adr_s.sin_addr.s_addr = htonl(INADDR_ANY);

bind (sock, (struct sockaddr *) &adr_s, sizeof(adr_s)); // Attachement socket
while (1)
{ lg = sizeof(adr_c);
n = recvfrom (sock, buf, 20, 0, (struct sockaddr *) &adr_c, &lg);
printf("Message reçu : %s \n", buf); } close(sock); }
