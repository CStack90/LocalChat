#include "../utils/colors.h"

#include "commands.h"

void printHelp() {
    printf(C_YEL);
    printf("%s -> Log in to the server\n", LOGIN_COMMAND);
    printf("%s -> Log out from the server\n", LOGOUT_COMMAND);
    printf("%s -> Create an account\n", CREATE_ACCOUNT_COMMAND);
    printf("%s -> Delete an account\n", DELETE_ACCOUNT_COMMAND);
    printf("%s -> Get a list of connected users\n", LIST_COMMAND);
    printf("%s -> Quit the program\n", EXIT_COMMAND);
    printf(C_RESET);
}

int is_command(char* message, char* command){
    if((strncmp(message,command,strlen(command)) == 0) 
        && (message[strlen(command)] == ' ' || message[strlen(command)] == '\0')){
        return 1;
    }
    return 0;
}

void login(char message[REQUEST_DATA_MAX_LENGTH],char* token, struct sockaddr_in adr_s, int udp_socket, int tcp_socket){
    struct request request;
    unsigned int lg = sizeof(adr_s);
    /* Building request */
    request.type = 1;
    strcpy(request.data,&message[strlen(LOGIN_COMMAND)+1]);

    /* Log in request (UDP) */
    sendto (udp_socket, (void *) &request, sizeof(struct request), 0, (struct sockaddr *) &adr_s, sizeof(adr_s)); 

    /* Receiving token */
    if (recvfrom (udp_socket, &request, sizeof(struct request), 0, (struct sockaddr *) &adr_s, &lg) > 0) {
        if (request.type == 0) { // Send token to tcp server
            strcpy(token, request.data);
            write(tcp_socket, request.data, strlen(request.data));
        } else { // Wrong username/login
            printf(C_RED "%s\n" C_RESET, request.data);
        }
    }
}

void logout(char* token, struct sockaddr_in adr_s, int udp_socket, int tcp_socket, int* exit_status){
    struct request request;
    unsigned int lg = sizeof(adr_s);
    /* Building request */
    request.type = -1;
    strcpy(request.data,token);

    /* Send request */
    sendto (udp_socket, (void *) &request, sizeof(struct request), 0, (struct sockaddr *) &adr_s, sizeof(adr_s));

    /* Receiving confirmation */
    if (recvfrom (udp_socket, &request, sizeof(struct request), 0, (struct sockaddr *) &adr_s, &lg) > 0) {
        if (request.type == 0) { //Send deconnection to tcp server
            write(tcp_socket, LOGOUT_COMMAND, strlen(LOGOUT_COMMAND));
            *exit_status = 1;
        } else { // Wrong token
            printf(C_RED "%s\n" C_RESET, request.data);
        }
    }
}

void createAccount(char message[REQUEST_DATA_MAX_LENGTH], struct sockaddr_in adr_s, int udp_socket){
    struct request request;
    unsigned int lg = sizeof(adr_s);
    /* Building request */
    request.type = 2;
    strcpy(request.data,&message[strlen(CREATE_ACCOUNT_COMMAND)+1]);

    /* Log in request (UDP) */
    sendto (udp_socket, (void *) &request, sizeof(struct request), 0, (struct sockaddr *) &adr_s, sizeof(adr_s)); 

    /* Receiving token */
    if (recvfrom (udp_socket, &request, sizeof(struct request), 0, (struct sockaddr *) &adr_s, &lg)>0){
        if (request.type == 0) {
            printf(C_GRN "%s" C_RESET "\n",request.data);
        } else { // Something went wrong
            printf(C_RED "Syntax: /create User Password\n");
            printf("%s" C_RESET "\n",request.data);
        }
    }
}

void deleteAccount(char message[REQUEST_DATA_MAX_LENGTH], struct sockaddr_in adr_s, int udp_socket){
    struct request request;
    unsigned int lg = sizeof(adr_s);
    /* Building request */
    request.type = -2;
    strcpy(request.data,&message[strlen(DELETE_ACCOUNT_COMMAND)+1]);

    /* Log in request (UDP) */
    sendto (udp_socket, (void *) &request, sizeof(struct request), 0, (struct sockaddr *) &adr_s, sizeof(adr_s)); 

    /* Receiving token */
    if (recvfrom (udp_socket, &request, sizeof(struct request), 0, (struct sockaddr *) &adr_s, &lg)>0){
        if(request.type == 0){
            printf(C_GRN "%s" C_RESET "\n",request.data);
        }else{ //Something went wrong
            printf(C_RED "Syntax: /delete User Password\n");
            printf("%s\n" C_RESET, request.data);
        }
    }
}

void connectedUsers(struct sockaddr_in adr_s, int udp_socket){
    struct request request;
    unsigned int lg = sizeof(adr_s);
    int counter = 1;

    /* Building request */
    request.type = 0;
    strcpy(request.data,"");

    /* Send request */
    sendto (udp_socket, (void *) &request, sizeof(struct request), 0, (struct sockaddr *) &adr_s, sizeof(adr_s));

    /* Receiving list */
    if (recvfrom (udp_socket, &request, sizeof(struct request), 0, (struct sockaddr *) &adr_s, &lg) > 0) {
        if (request.type == 0 && strlen(request.data) > 0) { // Display connected users
            /* Print users username */
            printf(C_YEL "User 1: ");
            for (size_t i = 0; i < strlen(request.data); i++)
            {
                if (request.data[i] == '\t') {
                    counter++;
                    printf("\nUser %d: ", counter);
                } else {
                    printf("%c", request.data[i]);
                }
            }
            printf(C_RESET "\n");
        } else if (request.type == 0) {
            printf("Nobody is connected\n");
        } else { // Something went wrong
            printf(C_RED "%s\n" C_RESET, request.data);
        }
    }

}

int commande_detection(char message[REQUEST_DATA_MAX_LENGTH], int* exit_status, char* token, int tcp_sock){
    /* ---UDP connection--- */
    struct sockaddr_in adr_s, adr_c; //server and client addresses
    unsigned int sock; //Socket

    if (strlen(message) > 0 && message[0] == '/') {
        /* Addresses init */
        //Client init
        bzero(&adr_c,sizeof(adr_c));
        adr_c.sin_family = AF_INET; 
        adr_c.sin_port = htons(UDP_PORT);
        adr_c.sin_addr.s_addr = htonl(INADDR_ANY);
        //Server init
        bzero(&adr_s,sizeof(adr_s));
        adr_s.sin_family = AF_INET;
        adr_s.sin_port = htons(UDP_PORT);
        adr_s.sin_addr.s_addr = htonl(INADDR_ANY);

        /* Socket creation and binding */
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        bind(sock, (struct sockaddr *) &adr_c, sizeof(adr_c));

        if (is_command(message,LOGIN_COMMAND)) {
            if (strcmp(token,"") != 0) {
                printf(C_RED "You are already logged in" C_RESET "\n");
                return 1;
            }
            if (message[strlen(LOGIN_COMMAND)] == '\0') {
                printf(C_RED "Syntax: /login MyUser MyPass" C_RESET "\n");
            } else {
                //Necessite TCP_socket to send message
                login(message,&(*token), adr_s, sock, tcp_sock);
            }
        } else if (is_command(message,LOGOUT_COMMAND)) {
            //Necessite TCP_socket to send message
            logout(token, adr_s, sock, tcp_sock, exit_status);
        } else if (is_command(message,CREATE_ACCOUNT_COMMAND)) {
            createAccount(message,adr_s,sock);
        } else if (is_command(message,DELETE_ACCOUNT_COMMAND)) {
            if (strcmp(token, "") != 0) {
                printf(C_RED "You must be logged out to delete an account" C_RESET "\n");
                return 1;
            }
            deleteAccount(message,adr_s,sock);
        } else if (is_command(message,LIST_COMMAND)) {
            connectedUsers(adr_s,sock);
        } else if (is_command(message,EXIT_COMMAND)) {
            *exit_status = 1;
        } else if (is_command(message,HELP_COMMAND)) {
            printHelp();
        } else {
            printf(C_RED "Command not recognized\n" C_RESET);
        }
        return 1;
    }
    return 0;
}