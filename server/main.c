#include <signal.h>
#include <errno.h>

#include "../utils/utils.h"
#include "../utils/request.h"
#include "../utils/signals.h"
#include "../utils/client-structures.h"
#include "user_management.h"
#include "request_management.h"

extern int errno;
static int IS_RUNNING = true;
int sock_s;
struct user *shared_memory; //Shared memory variable (connected users)

static void handler(int sig, siginfo_t *info, void *ctx) {
    printf("Received signal %s (%d)\n", get_signal_name(sig), sig);
    IS_RUNNING = false;
    sigaction(sig, &noaction, NULL);
    shutdown(sock_s, SHUT_RD);
    close(sock_s);
    /* Wait for loops or whatever is running to finish */
    sleep(1);
    /* Unmap shared_memory */
    munmap(shared_memory, MAX_USERS_CONNECTED*sizeof(char*));
    kill(0, sig);
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
*\brief Send a message to every connected users
*
*\param message Message to send to everybody
*\param shared_memory Connected users
*\param sender_memory_index Index in shared_memory of sender users
 */
void broadcastMessage(char message[REQUEST_DATA_MAX_LENGTH], struct user *shared_memory, int sender_memory_index){
    // char broadcast_message[REQUEST_DATA_MAX_LENGTH+MAX_USER_USERNAME_LENGTH+2];// USERNAME: message
    /* Build message adding username */
    // sprintf(broadcast_message,"%s: %s",shared_memory[sender_memory_index].username,message);

    tcpData broadcast_message;
    broadcast_message.type = 1;
    strcpy(broadcast_message.username, shared_memory[sender_memory_index].username);
    strcpy(broadcast_message.message, message);

    /* Send message to every connected users */
    for (size_t i = 0; i < MAX_USERS_CONNECTED; i++)
    {
        if (shared_memory[i].sock != 0) {
            send(shared_memory[i].sock, &broadcast_message, sizeof(broadcast_message),0);
        }
    }
}

/**
*\brief Thread consecrated to only one connection, it receives messages, manage login with token
* It also call the function to send the message to every connected users
*
*\param args 
*\return void* Nothing
*/
void* message_receiver(void* args) {
    struct tcp_informations *arguments = args; // Information from communication thread
    int sock_c = (*arguments).sock_c; // Client socket
    char message[REQUEST_DATA_MAX_LENGTH]; // Received message
    int message_length; // Message length 
    int memory_index = -1; // Index of the user in shared memory

    /* Wait to receive message */
    while (IS_RUNNING) {
        message_length = recv(sock_c, message, REQUEST_DATA_MAX_LENGTH, MSG_DONTWAIT);
        if (message_length < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) { // no message, wait and try again
            sleep(1);
            errno = 0;
            continue;
        }
        if (message_length < 1) // sometimes 0 when the pipe breaks
            break;
        message[message_length] = '\0';

        if (memory_index == -1) { // User not connected
            // Check is the token match to a user
            for (size_t i = 0; i < MAX_USERS_CONNECTED; i++)
            {
                if(strcmp(message,(*arguments).shared_memory[i].token) == 0) {
                    (*arguments).shared_memory[i].sock = sock_c;
                    memory_index = i;
                }
            }
            if (memory_index == -1) { // No token correspondance found
                tcpData msg = {7, "", "You need to log in to send messages!"};
                send(sock_c, &msg, sizeof(msg), 0);
            } else {
                tcpData msg = {5, "", "You have been successfully connected!"};
                send(sock_c, &msg, sizeof(msg), 0);
            }
        } else { //User connected
            if (strcmp(message, "/logout") == 0) { // Logout
                /* Sending /logout to disconnect client too */
                tcpData msg = {6, "", "/logout"};
                send(sock_c, &msg, sizeof(msg), 0);
                break;
            }
            // Normal message
            printf("Message received (%ld): %s\n", strlen(message), message);
            broadcastMessage(message, (*arguments).shared_memory, memory_index);
        }
    }
    printf("[client_thread] - Closing TCP connexion %d\n", memory_index);
    /* Close the connection and exit */
    close(sock_c);
    if (memory_index != -1 && (*arguments).shared_memory[memory_index].sock == sock_c) { // Check if still connected then clear the shared_memory
        (*arguments).shared_memory[memory_index].sock = 0;
        strcpy((*arguments).shared_memory[memory_index].token,"");
        strcpy((*arguments).shared_memory[memory_index].username,"");
    }
    pthread_exit(NULL);
}

/**
*\brief This function used by a thread will manage TCP connection affect a thread to each client
*
*\param args [struct user *] shared memory of connected users with the request manager thread
*\return void* Nothing
*/
void *communication(void* args){
    struct user *shared_memory = args;
    struct sockaddr_in adr_s; //Server address
    struct tcp_informations thread_infos;
    sock_s = socket( AF_INET , SOCK_STREAM, 0 );//Server socket
    int sock_c = 0; //Client socket
    pthread_t client_thread; //Thread for wait client messages

    /* Init server addresses */
    bzero(&adr_s,sizeof(adr_s));
    adr_s.sin_family = AF_INET;
    adr_s.sin_port = htons(TCP_PORT);
    adr_s.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* Server init */
    if (bind(sock_s, (struct sockaddr *)&adr_s, sizeof(adr_s)) == -1){
        perror("[Communication] - Cannot bind server");
        exit(EXIT_FAILURE);
    }

    if (listen(sock_s ,REQUEST_DATA_MAX_LENGTH) == -1){
        perror("[Communication] - listening failed");
        exit(EXIT_FAILURE);
    }

    while (IS_RUNNING) {
        /* Waiting a connection */
        if ((sock_c = accept(sock_s, (struct sockaddr*) NULL, NULL)) < 0 && IS_RUNNING)
            perror("[Communication] - Accept failed");
        if (!IS_RUNNING)
            break;
        
        /* Thread for receive all message from this client */
        //Build object to give data
        thread_infos.shared_memory = shared_memory;
        thread_infos.sock_c = sock_c;

        printf("[Communication] - Creation message receiver thread...");
        if (pthread_create(&client_thread, NULL, message_receiver, &thread_infos)) // Thread creation
            perror("[Client_thread] - Error during thread creation");
        else
            printf("Created\n");
    }

    /* Properly end the communication thread */
    pthread_exit(NULL);
}

/**
*\brief This function used by a thread will manage UDP connection for login/logout, creation/suppression of user, liste of connected users 
*
*\param args [struct user *] shared memory of connected users with the communication thread
*\return void* Nothing
*/
void *request_manager(void* args){
    struct user *shared_memory = args; //Connected users
    unsigned int sock, lg = sizeof(struct sockaddr_in); //socket and adr_c size
    struct sockaddr_in adr_s, adr_c; //Server and client addresses

    //Function parameter furing thread creation
    void* thread_function = NULL;

    //Object sent to thread 
    struct request_processing arguments;

    //Request object
    struct request request;

    //Thread for request processing
    pthread_t request_thread;

    /* Socket init */
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    bzero(&adr_s,sizeof(adr_s)); 
    adr_s.sin_family = AF_INET;
    adr_s.sin_port = htons(UDP_PORT);
    adr_s.sin_addr.s_addr = htonl(INADDR_ANY);
    bind (sock, (struct sockaddr *) &adr_s, sizeof(adr_s)); // Attachement socket

    while (IS_RUNNING) {
        //Waiting for a new message
        if (recvfrom (sock, &request, sizeof(struct request), 0, (struct sockaddr *) &adr_c, &lg)){

            //request_processing object building
            arguments.adr_client = adr_c;
            arguments.request = request;
            arguments.shared_memory = shared_memory;
            arguments.sock = sock;

            switch (request.type){
                case 1: //log in
                    printf("[Request_manager] - Log in thread creation...\n");
                    thread_function = login; // function for log in
                    break;
                case -1://log out
                    printf("[Request_manager] - Log out thread creation...\n");
                    thread_function = logout; // function for log out
                    break;
                case 2://creation of account
                    printf("[Request_manager] - Account-creation thread creation...\n");
                    thread_function = account_creation; // function for create an account
                    break;
                case -2://deletion of account
                    printf("[Request_manager] - Account-deletion thread creation...\n");
                    thread_function = account_deletion; // function for delete an account
                    break;
                default://connected users list
                    printf("[Request_manager] - connected_users thread creation...\n");
                    thread_function = connected_users; // function send list of connected users
                    break;
            }
            /* Thread creation for request treatment */
            if (pthread_create( &request_thread, NULL, thread_function, &arguments))
                printf("\nError during request_thread creation\n");
            else
                pthread_detach(request_thread);
        }
    }
    /* Close socket */
    close(sock);
    /* Exit thread */
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    pthread_t com, req; //communication process and request manager process
    
    //Init of random function (used for token generation)
    srand(time(NULL));

    // add signal handler for potentially-killing signals
    int signals[6] = {SIGSTOP, SIGABRT, SIGINT, SIGQUIT, SIGTERM, SIGTSTP};
    handle_signals(signals, sizeof(signals)/sizeof(signals[0]));

    /* Shared memory of connected users initialization */
    shared_memory = mmap(NULL, MAX_USERS_CONNECTED*sizeof(struct user), (PROT_READ | PROT_WRITE), (MAP_SHARED | MAP_ANONYMOUS), -1, 0); //Shared memory init
    //Init of users array to empty string as username and ip_add
    for (size_t i = 0; i < MAX_USERS_CONNECTED; i++){
        strcpy(shared_memory[i].username,"");
        strcpy(shared_memory[i].token,"");
        shared_memory[i].sock = 0;
    }
    
    /* Communication thread creation */
    printf("Creation communication thread... ");
    if (pthread_create(&com, NULL, communication, (void*)shared_memory)) {
        printf("\nError during thread creation\n");
        exit(EXIT_FAILURE);
    } else
        printf("Created\n");

    /* Request manager thread creation */
    printf("Creation request thread... ");
    if (pthread_create(&req, NULL, request_manager, (void*)shared_memory)) {
        printf("\nError during thread creation\n");
        exit(EXIT_FAILURE);
    } else
        printf("Created\n");
    
    printf("Server is ready to serve!\n");

    /* Join communication and request manager threads */
    pthread_join(com, NULL);
    pthread_join(req, NULL);

    /* Unmap shared_memory */
    munmap(shared_memory, MAX_USERS_CONNECTED*sizeof(char*));

    printf("Server finished.\n");
    return 0;
}
