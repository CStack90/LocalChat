#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>

#include <sys/msg.h>
#include <unistd.h>

#include "commands.h"
#include "client_methods.h"
#include "../utils/utils.h"
#include "../utils/request.h"
#include "../utils/signals.h"
#include "../utils/client-structures.h"
#include "../utils/colors.h"

#if __APPLE__
    #define OPEN_BOARD "open ./output/board --env MSGID=%i"
#else
    #define OPEN_BOARD "export MSGID=%i; gnome-terminal -- \"./output/board\""
#endif

int msgid;
int tcp_socket;

static void handler(int sig, siginfo_t *info, void *ctx) {
    printf(C_RED "Received signal %s (%d)" C_RESET "\n", get_signal_name(sig), sig);
    // close board and message queue
    kill_board(msgid);
    msgctl(msgid, IPC_RMID, NULL);
    close(tcp_socket);
    exit(-1);
}

static void handle_signals(int signals[], int count) {
    struct sigaction action;
    memset(&action, '\0', sizeof(action));
    action.sa_sigaction = &handler;

    for (int i=0; i<count; i++) {
        sigaction(signals[i], &action, NULL);
    }
}

/**
*\brief Thread which receive each message from the server and will transfer it to board processus
*
*\param socket TCP socket to listen for new messages
*\return void* Nothing
*/
void *receive_msg()
{
    int len;
    // client thread always ready to receive message
    tcpData message;
    while ((len = recv(tcp_socket, &message, sizeof(message), 0)) > 0) {
        message.message[len] = '\0';
        /* If connection ended */
        if(strcmp(message.message, LOGOUT_COMMAND) == 0){
            printf(C_GRN "You are now logged out" C_RESET "\n");
            //TODO : end connexion to pipe
            break;
        }

        /* Send it to message queue */
        switch (message.type) {
            case 1: // messahe se,y
                sendMessage(msgid, message.username, message.message);
                break;
            case 4: // STOP
                kill_board(msgid);
                msgctl(msgid, IPC_RMID, NULL);
                break;
            case 5: // connected
                printf(C_GRN "You are now logged in" C_RESET "\n");
                sendSignal(msgid, 1);
                break;
            case 6: // disconnected
                printf(C_GRN "You are now disconnected" C_RESET "\n");
                sendSignal(msgid, 2);
                break;
            case 7: // you need to log in
                printf(C_RED "You need to log in first!" C_RESET "\n");
                break;
            default:
                printf(C_RED "Unknwon message from the server: [%ld] %s\n" C_RESET, message.type, message.message);
                break;
        }
    }
    return NULL;
}

/**
*\brief Creation of TCP socket and interception of each connection to affect a thread to connexion
*
*\param args NULL
*\return void* Nothing
 */
void *TCP_connexion(void* args){
    char message[REQUEST_DATA_MAX_LENGTH]; //Message wrote by user
    tcp_socket = socket( AF_INET, SOCK_STREAM,0); //Client socket
    struct sockaddr_in adr_s; //Server address
    int exit_status = 0; //Exit while condition
    pthread_t receiver; //Thread that will receive messages
    char token[TOKEN_SIZE]; //Connexion token
    strcpy(token, "");

    /* Server address init */
    bzero(&adr_s,sizeof(adr_s));
    adr_s.sin_port = htons(TCP_PORT);
    adr_s.sin_family= AF_INET;
    adr_s.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* Establish the connection */
    if ((connect(tcp_socket ,(struct sockaddr *)&adr_s,sizeof(adr_s))) == -1 ) {
        perror("Connection to socket failed");
        kill(0, SIGINT);
        pthread_exit(NULL);
    }

    // create a thread to receive messages from server
    pthread_create(&receiver, NULL, receive_msg, NULL);
    printHelp(); // print help menu

    /* Sending messages */
    while (exit_status == 0) {
        saisieString(message, REQUEST_DATA_MAX_LENGTH);
        if (commande_detection(message, &exit_status, &(*token), tcp_socket) == 0){ //There is no command
            write(tcp_socket, message, strlen(message));
        }
    }

    /* Properly end the client */
    close(tcp_socket);
    printf("[TCP-connexion] - Connection ended !\n");
    pthread_exit(NULL);
}

static int create_msg_pipe() {
    int msgid = msgget(IPC_PRIVATE, IPC_CREAT|IPC_EXCL|0640);
    if (msgid == -1) {
        perror("Unable to create message pipe:");
        msgctl(msgid, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }
    return msgid;
}


int main(int argc, char const *argv[]) {
    pthread_t tcp_connect; // TCP connection
    
    // add signal handler for potentially-killing signals
    int signals[6] = {SIGSTOP, SIGABRT, SIGINT, SIGQUIT, SIGTERM, SIGTSTP};
    handle_signals(signals, sizeof(signals)/sizeof(signals[0]));

    // message pipe test
    msgid = create_msg_pipe();   

    // launch board console in a new terminal
    char command[200];
    sprintf(command, OPEN_BOARD, msgid);
    if (system(command) != 0) {
        fprintf(stderr, C_RED "Unable to open the board console: abort\n" C_RESET);
        return EXIT_FAILURE;
    }
  
    /* Creation of TCP connexion manager */
    printf("Creating TCP thread... ");
    if (pthread_create( &tcp_connect, NULL, TCP_connexion, NULL)) {
        perror("\nError during thread creation");
        kill_board(msgid);
        msgctl(msgid, IPC_RMID, NULL);
    } else
        printf("Created\n");

    /* Join TCP connexion manager thread */
    pthread_join(tcp_connect, NULL);

    kill_board(msgid);
    sleep(2);

    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}
