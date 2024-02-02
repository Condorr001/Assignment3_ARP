#include "constants.h"
#include "wrapFuncs/wrapFunc.h"
#include <curses.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <utils/utils.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// WD pid
pid_t WD_pid;

// Boolean to decide whether to use pipes or sockets
bool socket_use = false;

// Once the SIGUSR1 is received send back the SIGUSR2 signal
void signal_handler(int signo, siginfo_t *info, void *context) {
    // Specifying thhat context is not used
    (void)(context);

    if (signo == SIGUSR1) {
        WD_pid = info->si_pid;
        Kill(WD_pid, SIGUSR2);
    }
}

int main(int argc, char *argv[]) {
    // Signal declaration
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    // Setting the signal handler
    sa.sa_sigaction = signal_handler;
    // Resetting the mask
    sigemptyset(&sa.sa_mask);
    // Setting flags
    // The SA_RESTART flag has been added to restart all those syscalls that can
    // get interrupted by signals
    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    Sigaction(SIGUSR1, &sa, NULL);

    // Specifying that argc and argv are unused variables
    int to_server_pipe, from_server_pipe;

    //socket initialization
    int client_fd;
    if (socket_use) {
        struct sockaddr_in server_addr;

        //create the socket
        client_fd = Socket(AF_INET, SOCK_STREAM, 0);

        //defining the server address for the socket
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);

        //convert addresses from text to binary
        Inet_pton(AF_INET, SOCKET_ADDRESS, &server_addr.sin_addr);

        //connect the socket to the specified address
        Connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    }

    else {
        //pipes initialization
        if (argc == 3) {
            sscanf(argv[1], "%d", &to_server_pipe);
            sscanf(argv[2], "%d", &from_server_pipe);
        } else {
            printf("Wrong number of arguments in target\n");
            getchar();
            exit(1);
        }
    }

    // coordinates of target
    float target_x, target_y;
    // string for the communication with server
    char to_send[MAX_MSG_LEN] = "T";
    // String to compose the message for the server
    char aux_to_send[MAX_MSG_LEN] = {0};

    // seeding the random nymber generator with the current time, so that it
    // starts with a different state every time the programs is executed
    srandom((unsigned int)time(NULL));

    char received[MAX_MSG_LEN];

    while (1) {
        // Resetting to_send string
        sprintf(to_send, "T");
        // spawn random coordinates in map field range and send it to the
        // server, so that they can be spawned in the map
        sprintf(aux_to_send, "[%d]", N_TARGETS);
        strcat(to_send, aux_to_send);
        for (int i = 0; i < N_TARGETS; i++) {
            if (i != 0) {
                strcat(to_send, "|");
            }
            // Targets need to be inside the simulation window
            target_x = random() % SIMULATION_WIDTH;
            target_y = random() % SIMULATION_HEIGHT;
            sprintf(aux_to_send, "%.3f,%.3f", target_x, target_y);
            strcat(to_send, aux_to_send);
        }

        if (socket_use)
            // send the obstacles coordinates to the other program
            Send(client_fd, to_send, MAX_MSG_LEN, 0);
        else
            // sending new targets to the server
            Write(to_server_pipe, to_send, MAX_MSG_LEN);

        if (!socket_use) {
            // Here Read is used instead of Select because it has to be blocking
            // untill the server sends a new GE
            Read(from_server_pipe, received, MAX_MSG_LEN);
            if (!strcmp(received, "GE")) {
                logging(LOG_INFO, "Received GE");
            } 
            else if (!strcmp(received, "STOP")) {
                // Otherwise if it's STOP close everything
                break;
            }
        }

        else {
            //TO DO: read from socket and see if GE or STOP are received
        }
        // Log if new targets have been produced
        logging(LOG_INFO, "Target process generated a new set of targets");
    }

    // Cleaning up
    Close(to_server_pipe);
    return 0;
}
