#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#include "../utils/client-structures.h"
#include "../utils/utils.h"
#include "./threads.h"

int get_msgid() {
    int buffer_size = 100;
    char msgid_str[buffer_size];
    char *envvar = "MSGID";
    // Make sure envar actually exists
    if(!getenv(envvar)){
        fprintf(stderr, "The environment variable %s was not found.\n", envvar);
        exit(1);
    }

    // Make sure the buffer is large enough to hold the environment variable value.
    if(snprintf(msgid_str, buffer_size, "%s", getenv(envvar)) >= buffer_size){
        fprintf(stderr, "BUFSIZE of %d was too small. Aborting\n", buffer_size);
        exit(1);
    }
    // Make sure the env value is actually a usable integer
    if (!isNumber(msgid_str)) {
        fprintf(stderr, "%s value is not a valid integer. Aborting\n", msgid_str);
        exit(1);
    }
    return atoi(msgid_str);
}

int main(int argc, char const *argv[])
{
    int msgid = get_msgid();
    pthread_t thread1, thread2, thread3;
    int status;
    
    // create threads to receive messages
    // Thread 1: signal handler
    status = pthread_create(&thread1, NULL, collectSignals, (void*) (size_t) msgid);
    if (status != 0) {
        perror("Error creating thread 1");
    }
    // Thread 2: messages display
    status = pthread_create(&thread2, NULL, collectMessages, (void*) (size_t) msgid);
    if (status != 0) {
        perror("Error creating thread 2");
    }
    // Thread 3: members joining and leaving
    status = pthread_create(&thread3, NULL, collectMoves, (void*) (size_t) msgid);
    if (status != 0) {
        perror("Error creating thread 3");
    }
    
    // join first thread
    pthread_join(thread1, NULL);
    
    // when thread 1 has finished, we can kill the others
    pthread_kill(thread2, SIGKILL);
    pthread_kill(thread3, SIGKILL);

    return EXIT_SUCCESS;
}
