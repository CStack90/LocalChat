// define functions used by board threads (one per message type)

#include "threads.h"
#include <pthread.h>
#include <stdio.h>
#include <sys/msg.h>

#include "../utils/colors.h"

void* collectMessages(void* arg) {
    int msgid = (size_t) arg;
    messageSignal req;
    size_t size = sizeof(req) - sizeof(long);
    while (1) {
        if (msgrcv(msgid, &req, size, 1, 0) == -1) {
            perror("P1: Error receiving request");
            return NULL;
        }
        printf(C_CYN "%s:" C_RESET " %s\n", req.username, req.message);
    }
    pthread_exit(0);
}

void* collectMoves(void* arg) {
    int msgid = (size_t) arg;
    moveSignal req;
    size_t size = sizeof(req) - sizeof(long);
    while (1) {
        if (msgrcv(msgid, &req, size, 2, 0) == -1) {
            perror("P2: Error receiving request");
            return NULL;
        }
        if (req.is_join)
            printf("%s joined the chat\n", req.username);
        else
            printf("%s left the chat\n", req.username);
    }
    pthread_exit(0);
}

void* collectSignals(void* arg) {
    int msgid = (size_t) arg;
    stopSignal req;
    size_t size = sizeof(req) - sizeof(long);
    while (1) {
        if (msgrcv(msgid, &req, size, 3, 0) == -1) {
            perror("P3: Error receiving request");
            return NULL;
        }
        switch (req.signal) {
            case 0:
                printf("Closing console...\n");
                // exit(EXIT_SUCCESS);
                pthread_exit(0);
                return NULL;
            case 1:
                printf("You joined the chat\n");
                break;
            case 2:
                printf("You left the chat\n");
                break;
            default:
                printf("unknown signal\n");
        }
    }
    pthread_exit(0);
}