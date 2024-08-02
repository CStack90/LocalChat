#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#define PORT 2058
int main() { // client
int sock; struct sockaddr_in adr_s, adr_c;
sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Creation socket
bzero(&adr_s,sizeof(adr_s));
adr_s.sin_family = AF_INET;
adr_s.sin_port = htons(PORT);
adr_s.sin_addr.s_addr = inet_addr("127.0.0.1");
connect(sock, (struct sockaddr *) &adr_s, sizeof(adr_s));
write(sock, "bonjour, serveur !!!", 20);
close(sock); }
