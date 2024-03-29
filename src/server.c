#include "constants.h"
#include "dataStructs.h"
#include "utils/utils.h"
#include "wrapFuncs/wrapFunc.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // Port on which to expose the services
    int PORT = get_param("server", "server_port");

    // Specifying that argc and argv are unused variables
    uint from_drone_pipe, to_drone_pipe, from_input_pipe, to_input_pipe,
        from_map_pipe, to_map_pipe;

    if (argc == 7) {
        sscanf(argv[1], "%u", &from_drone_pipe);
        sscanf(argv[2], "%u", &to_drone_pipe);
        sscanf(argv[3], "%u", &from_input_pipe);
        sscanf(argv[4], "%u", &to_input_pipe);
        sscanf(argv[5], "%u", &from_map_pipe);
        sscanf(argv[6], "%u", &to_map_pipe);
    } else {
        printf("Server: wrong number of arguments in input\n");
        fflush(stdout);
        getchar();
        exit(1);
    }

    // Structs for each drone information
    struct pos drone_current_pos           = {0};
    struct velocity drone_current_velocity = {0};

    // Declaring the logfile aux buffer
    char received[MAX_MSG_LEN];
    char to_send[MAX_MSG_LEN];

    fd_set reader;
    fd_set master;
    // Resetting the fd_sets
    FD_ZERO(&reader);
    FD_ZERO(&master);
    FD_SET(from_drone_pipe, &master);
    FD_SET(from_input_pipe, &master);
    FD_SET(from_map_pipe, &master);

    // Setting the maxfd
    uint maxfd = max(max(from_drone_pipe, from_input_pipe),
                     max(from_map_pipe, from_map_pipe));

    bool to_exit = false;

    // NET CODE
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    uint listener;
    uint addrlen;
    // Array where to save sockfds
    uint to_identify[2];
    // Index to use to_identify as a stack
    uint to_identify_index = 0;
    // Variables where to save sockfds
    uint target_sockfd     = 0;
    uint obstacle_sockfd   = 0;

    listener                    = Socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(PORT);
    server_addr.sin_family      = AF_INET;

    bind(listener, (struct sockaddr *)&server_addr, sizeof(server_addr));

    listen(listener, 10);

    FD_SET(listener, &master);
    if (listener > maxfd)
        maxfd = listener;

    while (1) {
        // perform the select
        reader = master;
        Select_wmask(maxfd + 1, &reader, NULL, NULL, NULL);

        // check the value returned by the select and perform actions
        // consequently
        for (uint i = 0; i <= maxfd; i++) {
            if (FD_ISSET(i, &reader)) {
                int ret = -1;
                // If the file descriptor that triggered the select is a socket
                // then read with echo. Otherwise perform normal read
                if (i == to_identify[0] || i == to_identify[1] ||
                    i == target_sockfd || i == obstacle_sockfd)
                    ret = Read_echo(i, received, MAX_MSG_LEN);
                else if (i != listener)
                    ret = Read(i, received, MAX_MSG_LEN);
                // If the number of byte read is 0 then the socket or pipe has
                // been closed
                if (ret == 0) {
                    printf("Pipe or socket to server closed\n");
                    fflush(stdout);

                    // Close this side and remove it from the checked set
                    Close(i);
                    FD_CLR(i, &master);

                    // If it's a socket decrease to_identify_index that is a
                    // variable that allows the to_identify array to be used
                    // like a stack
                    if (i == to_identify[0] || i == to_identify[1] ||
                        i == obstacle_sockfd || i == target_sockfd)
                        to_identify_index--;
                } else {
                    // Here new connections are opened
                    if (i == listener) {
                        addrlen = sizeof(client_addr);

                        // Here the sockfd that still needs to be identified is
                        // saved in the array
                        to_identify[to_identify_index] =
                            accept(listener, (struct sockaddr *)&client_addr,
                                   &addrlen);

                        // The sockfd is then added into the fdset
                        FD_SET(to_identify[to_identify_index], &master);

                        // Update maxfd variable
                        if (to_identify[to_identify_index] > maxfd)
                            maxfd = to_identify[to_identify_index];

                        // Keep track of the top of the array
                        to_identify_index++;
                        // Prevent going out of bounds
                        if(to_identify_index > 2){
                            printf("Too many connections. Please restart");
                            fflush(stdout);
                            exit(1);
                        }
                    } else if (i == to_identify[0]) {
                        char dim[MAX_MSG_LEN];
                        // Send the size of the simulation window
                        sprintf(dim, "%.3f,%.3f", (float)SIMULATION_HEIGHT,
                                (float)SIMULATION_WIDTH);
                        // Assign the file descriptor according to the
                        // initialization message received
                        if (!strcmp(received, "TI")) {
                            Write_echo(i, dim, MAX_MSG_LEN);
                            target_sockfd = i;
                        } else if (!strcmp(received, "OI")) {
                            Write_echo(i, dim, MAX_MSG_LEN);
                            obstacle_sockfd = i;
                        } else {
                            logging(
                                LOG_ERROR,
                                "Wrong initialization message. Check protocol");
                        }
                        to_identify[0] = 0;
                    } else if (i == to_identify[1]) {
                        char dim[MAX_MSG_LEN];
                        // Send the size of the simulation window
                        sprintf(dim, "%.3f,%.3f", (float)SIMULATION_HEIGHT,
                                (float)SIMULATION_WIDTH);
                        // Assign the file descriptor according to the
                        // initialization message received
                        if (!strcmp(received, "TI")) {
                            Write_echo(i, dim, MAX_MSG_LEN);
                            target_sockfd = i;
                        } else if (!strcmp(received, "OI")) {
                            Write_echo(i, dim, MAX_MSG_LEN);
                            obstacle_sockfd = i;
                        } else {
                            logging(
                                LOG_ERROR,
                                "Wrong initialization message. Check protocol");
                        }
                        to_identify[1] = 0;
                    } else if (i == from_input_pipe) {
                        // If the user wants to stop the processes then forward
                        // it to all the others
                        if (!strcmp(received, "STOP")) {
                            Write(to_drone_pipe, "STOP", MAX_MSG_LEN);
                            Write(to_map_pipe, "STOP", MAX_MSG_LEN);
                            if (obstacle_sockfd)
                                Write_echo(obstacle_sockfd, "STOP",
                                           MAX_MSG_LEN);
                            if (target_sockfd)
                                Write_echo(target_sockfd, "STOP", MAX_MSG_LEN);
                            to_exit = true;
                            break;
                        } else {
                            // Otherwise send the drone position and velocity
                            // calculated from the drone process to the input
                            sprintf(to_send, "%f,%f|%f,%f", drone_current_pos.x,
                                    drone_current_pos.y,
                                    drone_current_velocity.x_component,
                                    drone_current_velocity.y_component);
                            Write(to_input_pipe, to_send, MAX_MSG_LEN);
                        }
                    } else if (i == from_drone_pipe) {
                        // The drone process sends the update speed and position
                        // of the drone
                        sscanf(received, "%f,%f|%f,%f", &drone_current_pos.x,
                               &drone_current_pos.y,
                               &drone_current_velocity.x_component,
                               &drone_current_velocity.y_component);
                        // Send the drone current position to the map
                        sprintf(to_send, "D%f|%f", drone_current_pos.x,
                                drone_current_pos.y);
                        Write(to_map_pipe, to_send, MAX_MSG_LEN);
                    } else if (i == from_map_pipe) {
                        logging(LOG_INFO, received);
                        if (!strcmp(received, "GE")) {
                            // If we have GE sent by the map then send to target
                            // so it can produce new targets
                            if (target_sockfd)
                                Write_echo(target_sockfd, "GE", MAX_MSG_LEN);
                        } else if (received[0] == 'T' && received[1] == 'H') {
                            // If TH then there has been a target hit, inform
                            // the drone in order to remove it from the targets
                            // to consider for the forces calculations
                            Write(to_drone_pipe, received, MAX_MSG_LEN);
                        }
                    } else if (i == obstacle_sockfd) {
                        // When new obstacles are ready inform map and drone
                        Write(to_map_pipe, received, MAX_MSG_LEN);
                        Write(to_drone_pipe, received, MAX_MSG_LEN);
                    } else if (i == target_sockfd) {
                        // When new targets are ready inform map and
                        // drone
                        Write(to_map_pipe, received, MAX_MSG_LEN);
                        Write(to_drone_pipe, received, MAX_MSG_LEN);
                    }
                }
            }
        }
        // If STOP sent then it needs to be closed
        if (to_exit)
            break;
    }
    // Close pipes
    Close(from_drone_pipe);
    Close(from_input_pipe);
    Close(from_map_pipe);
    Close(to_drone_pipe);
    Close(to_map_pipe);
    Close(to_input_pipe);
    return EXIT_SUCCESS;
}
