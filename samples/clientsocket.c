#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#define PORT 2058
int main() { // client
int sock; 
char buf[20];
struct sockaddr_in adr_s, adr_c;
sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // Creation socket
bzero(&adr_c,sizeof(adr_c));
adr_c.sin_family = AF_INET; 
adr_c.sin_port = htons(PORT);
adr_c.sin_addr.s_addr = htonl(INADDR_ANY);
bind (sock, (struct sockaddr *) &adr_c, sizeof(adr_c)); // Attachement socket
bzero(&adr_s,sizeof(adr_s)); adr_s.sin_family = AF_INET; adr_s.sin_port = htons(PORT);
adr_s.sin_addr.s_addr = htonl(INADDR_ANY);
sendto (sock, "bonjour, serveur !!!", 20, 0, (struct sockaddr *) &adr_s, sizeof(adr_s)); close(sock); }
