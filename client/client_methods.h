#include "../utils/client-structures.h"

void kill_board(int msgid);

void sendSignal(int msgid, int signal);

void sendMessage(int msgid, char* username, char* message);

void sendMove(int msgid, char* username, bool is_join);
