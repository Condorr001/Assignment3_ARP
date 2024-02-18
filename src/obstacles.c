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
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <utils/utils.h>

// Variables set to generate target and obstacles for a set dimension of the
// simulation window
float socket_simulation_height = 0;
float socket_simulation_width  = 0;

char received[MAX_MSG_LEN];

int main(int argc, char *argv[]) {
    // Specifying that argc and argv are not used
    (void)argc;
    (void)argv;
    
    // Server port to which to connect
    int PORT = get_param("target", "server_port");

    // socket initialization
    int server_fd;

    // Wait for the server to be up
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

    Write_echo(server_fd, "OI", MAX_MSG_LEN);

    // Get dimensions for which to generate obstacles
    char dimensions[MAX_MSG_LEN];
    Read_echo(server_fd, dimensions, MAX_MSG_LEN);

    // Parsing the string
    sscanf(dimensions, "%f,%f", &socket_simulation_height,
           &socket_simulation_width);

    // coordinates of obstacles
    float obstacle_x, obstacle_y;
    // string for the communication with server
    char to_send[MAX_MSG_LEN] = "O";
    // String to compose the message for the server
    char aux_to_send[MAX_MSG_LEN] = {0};

    // seeding the random nymber generator with the current time, so that it
    // starts with a different state every time the programs is executed
    srandom((unsigned int)time(NULL) * 33);

    fd_set reader, master;
    FD_ZERO(&reader);
    FD_ZERO(&master);
    FD_SET(server_fd, &master);

    struct timeval select_timeout;
    select_timeout.tv_sec  = OBSTACLES_SPAWN_PERIOD;
    select_timeout.tv_usec = 0;
    while (1) {
        // spawn random coordinates in map field range and send it to the
        // server, so that they can be spawned in the map, and format as
        // specified in the protocol
        sprintf(aux_to_send, "[%d]", N_OBSTACLES);
        strcat(to_send, aux_to_send);
        for (int i = 0; i < N_OBSTACLES; i++) {
            if (i != 0) {
                strcat(to_send, "|");
            }
            // The obstacle has to stay inside the simulation window
            obstacle_x = (float)random() /
                         (float)((float)RAND_MAX / socket_simulation_height);
            obstacle_y = (float)random() /
                         (float)((float)RAND_MAX / socket_simulation_width);
            sprintf(aux_to_send, "%.3f,%.3f", obstacle_x, obstacle_y);
            strcat(to_send, aux_to_send);
        }

        // send the obstacles coordinates to the other program
        Write_echo(server_fd, to_send, MAX_MSG_LEN);

        // Resetting to_send string
        sprintf(to_send, "O");

        // Logging the correct generation
        logging(LOG_INFO, "Obstacles process generated a new set of obstacles");

        // Resetting the fd_seta
        reader = master;
        int ret;
        do {
            ret = Select(server_fd + 1, &reader, NULL, NULL, &select_timeout);
        } while (ret == -1);
        // Resetting the timeout
        select_timeout.tv_sec  = OBSTACLES_SPAWN_PERIOD;
        select_timeout.tv_usec = 0;
        if (FD_ISSET(server_fd, &reader)) {
            int read_ret;
            read_ret = Read_echo(server_fd, received, MAX_MSG_LEN);
            if (read_ret == 0) {
                // If closed pipe close fd
                Close(server_fd);
                FD_CLR(server_fd, &master);
                logging(LOG_WARN, "Pipe to obstacles closed");
            }
            // If STOP received then stop everything
            if (!strcmp(received, "STOP")) {
                break;
            }
        }
    }

    return 0;
}
