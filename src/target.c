#include "constants.h"
#include "wrapFuncs/wrapFunc.h"
#include <arpa/inet.h>
#include <curses.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <utils/utils.h>

// WD pid
pid_t WD_pid;

// Variables set to generate target and obstacles for a set dimension of the
// simulation window
float socket_simulation_height = 0;
float socket_simulation_width  = 0;

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
    // Specify that argc and argv are not used
    (void)argc;
    (void)argv;

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

    // Server port to whitch to connect
    int PORT = get_param("target", "server_port");

    // socket initialization
    int server_fd;
    sleep(1);
    struct sockaddr_in server_addr;

    // create the socket
    server_fd = Socket(AF_INET, SOCK_STREAM, 0);

    // defining the server address for the socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(PORT);

    // convert addresses from text to binary
    Inet_pton(AF_INET, SERVER_ADDRESS, &server_addr.sin_addr);

    // connect the socket to the specified address
    Connect(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    Write_echo(server_fd, "TI", MAX_MSG_LEN);

    char dimensions[MAX_MSG_LEN];
    Read_echo(server_fd, dimensions, MAX_MSG_LEN);

    // Reading the dimensions of the window from the string sent by the
    // server
    sscanf(dimensions, "%fx%f", &socket_simulation_width,
           &socket_simulation_height);

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
            target_x = (float)random() /
                       (float)((float)RAND_MAX / socket_simulation_height);
            target_y = (float)random() /
                       (float)((float)RAND_MAX / socket_simulation_width);
            sprintf(aux_to_send, "%.3f,%.3f", target_x, target_y);
            strcat(to_send, aux_to_send);
        }

        // send the obstacles coordinates to the other program
        Write_echo(server_fd, to_send, MAX_MSG_LEN);

        // TO DO: read from socket and see if GE or STOP are received
        Read_echo(server_fd, received, MAX_MSG_LEN);
        if (!strcmp(received, "GE")) {
            logging(LOG_INFO, "Received GE");
        } else if (!strcmp(received, "STOP")) {
            // Otherwise if it's STOP close everything
            break;
        }
        // Log if new targets have been produced
        logging(LOG_INFO, "Target process generated a new set of targets");
    }

    return 0;
}
