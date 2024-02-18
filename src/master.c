#include "constants.h"
#include "wrapFuncs/wrapFunc.h"
#include <curses.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

bool lab_time = false;

// Function to spawn the processes
static void spawn(char **arg_list) {
    Execvp(arg_list[0], arg_list);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    // Specifying that argc and argv are unused variables
    (void)(argc);
    (void)(argv);

    // Define an array of strings for every process to spawn
    char programs[NUM_PROCESSES][20];
    strcpy(programs[0], "./server");
    strcpy(programs[1], "./drone");
    strcpy(programs[2], "./input");
    strcpy(programs[3], "./map");
    strcpy(programs[4], "./target");
    strcpy(programs[5], "./obstacles");

    // Pids for all children
    pid_t child[NUM_PROCESSES];

    // Arrays to contain the pids
    int input_drone[2];
    int server_drone[2];
    int drone_server[2];
    int server_input[2];
    int input_server[2];
    int map_server[2];
    int server_map[2];

    // Creating the pipes
    Pipe(input_drone);
    Pipe(server_drone);
    Pipe(drone_server);
    Pipe(server_input);
    Pipe(input_server);
    Pipe(map_server);
    Pipe(server_map);

    // strings to pass pipe values as args
    char input_drone_str[10];
    char drone_server_str[10];
    char server_drone_str[10];
    char server_input_str[10];
    char input_server_str[10];
    char map_server_str[10];
    char server_map_str[10];

    for (int i = 0; i < NUM_PROCESSES; i++) {
        child[i] = Fork();
        if (!child[i]) {

            // Spawn the input and map process using konsole
            char *arg_list[] = {programs[i], NULL, NULL, NULL, NULL, NULL,
                                NULL,        NULL, NULL, NULL, NULL, NULL};
            char *konsole_arg_list[] = {"konsole", "-e", programs[i], NULL,
                                        NULL,      NULL, NULL};

            switch (i) {
                case 0:
                    // Server
                    sprintf(drone_server_str, "%d", drone_server[0]);
                    sprintf(server_drone_str, "%d", server_drone[1]);
                    sprintf(input_server_str, "%d", input_server[0]);
                    sprintf(server_input_str, "%d", server_input[1]);
                    sprintf(map_server_str, "%d", map_server[0]);
                    sprintf(server_map_str, "%d", server_map[1]);
                    arg_list[1]  = drone_server_str;
                    arg_list[2]  = server_drone_str;
                    arg_list[3]  = input_server_str;
                    arg_list[4]  = server_input_str;
                    arg_list[5]  = map_server_str;
                    arg_list[6]  = server_map_str;
                    Close(drone_server[1]);
                    Close(server_drone[0]);
                    Close(input_server[1]);
                    Close(server_input[0]);
                    Close(map_server[1]);
                    Close(server_map[0]);
                    Close(input_drone[0]);
                    Close(input_drone[1]);
                    spawn(arg_list);
                    break;
                case 1:
                    // Drone
                    sprintf(input_drone_str, "%d", input_drone[0]);
                    sprintf(server_drone_str, "%d", server_drone[0]);
                    sprintf(drone_server_str, "%d", drone_server[1]);
                    arg_list[1] = input_drone_str;
                    arg_list[2] = server_drone_str;
                    arg_list[3] = drone_server_str;
                    Close(input_drone[1]);
                    Close(server_drone[1]);
                    Close(drone_server[0]);

                    Close(server_input[0]);
                    Close(server_input[1]);
                    Close(input_server[0]);
                    Close(input_server[1]);
                    Close(map_server[0]);
                    Close(map_server[1]);
                    Close(server_map[0]);
                    Close(server_map[1]);
                    spawn(arg_list);
                    break;
                case 2:
                    // Input
                    sprintf(input_drone_str, "%d", input_drone[1]);
                    sprintf(input_server_str, "%d", input_server[1]);
                    sprintf(server_input_str, "%d", server_input[0]);
                    konsole_arg_list[3] = input_drone_str;
                    konsole_arg_list[4] = input_server_str;
                    konsole_arg_list[5] = server_input_str;
                    Close(input_drone[0]);
                    Close(input_server[0]);
                    Close(server_input[1]);

                    Close(map_server[0]);
                    Close(map_server[1]);
                    Close(server_map[0]);
                    Close(server_map[1]);

                    Execvp("konsole", konsole_arg_list);
                    exit(EXIT_FAILURE);
                    break;
                case 3:
                    // Map
                    sprintf(map_server_str, "%d", map_server[1]);
                    sprintf(server_map_str, "%d", server_map[0]);
                    konsole_arg_list[3] = map_server_str;
                    konsole_arg_list[4] = server_map_str;
                    Close(map_server[0]);
                    Close(server_map[1]);


                    Execvp("konsole", konsole_arg_list);
                    exit(EXIT_FAILURE);
                    break;
                case 4:
                    // Target
                    if (!lab_time)
                        spawn(arg_list);
                    else
                        exit(EXIT_SUCCESS);
                    break;
                case 5:
                    // Obstacles
                    if (!lab_time)
                        spawn(arg_list);
                    else
                        exit(EXIT_SUCCESS);
                    break;
            }
        } else {
            // If we are in the father we need to close all the unused pipes
            // since they are duplicated every time
            switch (i) {
                case 1:
                    // Drone has spawned
                    Close(server_drone[0]);
                    Close(server_drone[1]);
                    Close(drone_server[0]);
                    Close(drone_server[1]);
                    break;
                case 2:
                    // Input has spawned
                    Close(server_input[0]);
                    Close(server_input[1]);
                    Close(input_server[0]);
                    Close(input_server[1]);
                    Close(input_drone[0]);
                    Close(input_drone[1]);
                    break;
                case 3:
                    // Map has spawned
                    Close(server_map[0]);
                    Close(server_map[1]);
                    Close(map_server[0]);
                    Close(map_server[1]);
                    break;
            }
        }
    }

    // Printing the pids
    printf("Server pid is %d\n", child[0]);
    printf("Drone pid is %d\n", child[1]);
    printf("Konsole of Input pid is %d\n", child[2]);
    printf("Konsole of Map pid is %d\n", child[3]);
    printf("Target pid is %d\n", child[4]);
    printf("Obstacles pid is %d\n", child[5]);

    // Value for waiting for the children to terminate
    int res;

    // Wait for all direct children to terminate. Map and the konsole on which
    // it runs on are not direct childs of the master process but of the server
    // one so they will not return here
    for (int i = 0; i < NUM_PROCESSES; i++) {
        int ret = Wait(&res);
        // Getting the exit status
        int status = 0;
        WEXITSTATUS(status);
        printf("Process %d terminated with code: %d\n", ret, status);
    }

    return EXIT_SUCCESS;
}
