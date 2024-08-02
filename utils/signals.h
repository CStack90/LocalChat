#include <stdio.h>
#include <string.h>
#include <signal.h>

struct sigaction noaction;

char* get_signal_name(int signal) {
    switch (signal) {
        case 2:
            return "SIGINT";
        case 3:
            return "SIGQUIT";
        case 6:
            return "SIGABRT";
        case 9:
            return "SIGKILL";
        case 15:
            return "SIGTERM";
        case 17:
            return "SIGSTOP";
        case 18:
            return "SIGTSTP";
        case 19:
            return "SIGCONT";
        default:
            return "UNKNOWN";
    }
}
