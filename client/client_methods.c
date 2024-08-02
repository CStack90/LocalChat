#include <string.h>
#include <sys/msg.h>

#include "client_methods.h"

void kill_board(int msgid) {
    stopSignal req = { 3, 0 };
    size_t s = sizeof(req) - sizeof(long);
    msgsnd(msgid, &req, s, 0);
}

void sendSignal(int msgid, int signal) {
    stopSignal req = {3, signal};
    msgsnd(msgid, &req, sizeof(req) - sizeof(long), 0);
}

void sendMessage(int msgid, char* username, char* message) {
    messageSignal req = {1, "", ""};
    strcpy(req.username, username);
    strcpy(req.message, message);
    msgsnd(msgid, &req, sizeof(req) - sizeof(long), 0);
}

void sendMove(int msgid, char* username, bool is_join) {
    moveSignal req = {2, "", is_join};
    strcpy(req.username, username);
    msgsnd(msgid, &req, sizeof(req) - sizeof(long), 0);
}