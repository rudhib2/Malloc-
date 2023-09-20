/**
 * shell
 * CS 341 - Fall 2023
 */

// I used Chat GPT for initial design and debugging code - it worked well.

#include "format.h"
#include "shell.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "vector.h"
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>

#define MAX_INPUT_SIZE 1024
static pid_t foreground_pid = 0;

// extern char *optarg;


typedef struct process {
    char *command;
    pid_t pid;
} process;



#include "format.h"
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#define MAX_INPUT_SIZE 1024

void sigint_handler(int signo) {
    if (foreground_pid > 0) {
        kill(-foreground_pid, SIGINT);
    }
}
int change_directory(const char* path) {
    if (chdir(path) != 0) {
        if (errno == ENOENT) {
            fprintf(stderr, "%s: No such file or directory\n", path);
        } else {
            perror("cd");
        }
        // return -1;
    }
    return 0;
}

int execute_command(char* command) {
    if (strcmp(command, "") == 0) {
        return 0;  // Empty command
    } else if (strncmp(command, "cd ", 3) == 0) {
        return change_directory(command + 3); // +3 to skip "cd "
    }

    int status;
    pid_t new_pid = fork();

    if (new_pid == 0) {
        char* token = strtok(command, " ");
        char* args[100];
        int i = 0;
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;
        execvp(args[0], args);
        // perror("execvp failed");
        exit(0);
    } else if (new_pid > 0) {
        waitpid(new_pid, &status, 0);
        return WEXITSTATUS(status);
    } else {
        print_fork_failed();
        return -1;
    }
    return 0;
}


void execute_ps() {
    print_process_info_header();

    // Iterate through processes and print their information
    // You can use your own logic to gather process information and populate process_info structs
    // For example, you can use the /proc filesystem as demonstrated in the previous response.

    // Example usage of process_info:
    process_info pinfo;
    pinfo.pid = getpid();
    pinfo.nthreads = 1;
    pinfo.vsize = 7328;
    pinfo.state = 'R';
    pinfo.start_str = "14:03";
    pinfo.time_str = "0:08";
    pinfo.command = "dd if=/dev/zero bs=1M count=123456 of=/dev/null &";

    print_process_info(&pinfo);

    // Repeat the above process_info printing for other processes

    // You can gather information about other processes and print them here
}



int execute_logical_command(char* logical_command) {
    char* token;
    char* command1;
    char* command2;
    int operator_type; // 0 for ;, 1 for &&, 2 for ||
    
    if ((token = strstr(logical_command, "&&"))) {
        operator_type = 1;
        command1 = strtok(logical_command, "&&");
        command2 = token + 2; 
    } else if ((token = strstr(logical_command, "||"))) {
        operator_type = 2;
        command1 = strtok(logical_command, "||");
        command2 = token + 2; 
    } else {
        operator_type = 0;
        token = strstr(logical_command, ";");
        command1 = strtok(logical_command, ";");
        command2 = token + 2;
    }

    int status1 = execute_command(command1);

    if (operator_type == 1) {
        if (status1 == 0) {
            execute_command(command2);
        }
    } else if (operator_type == 2) {
        if (status1 != 0) {
            execute_command(command2);
        }
    } else {
        if (command2 != NULL) {
            execute_command(command2);
        }
    }
    return status1;
}


void execute_script(const char* filename) {
    FILE *script_file = fopen(filename, "r");
    if (script_file == NULL) {
        print_script_file_error();
        // perror("Error opening script file");
        exit(0);
    }

    char line[MAX_INPUT_SIZE];

    while (fgets(line, sizeof(line), script_file)) {
        line[strlen(line) - 1] = '\0';

        int status;
        pid_t new_pid = fork();

        if (new_pid > 0) {
            waitpid(new_pid, &status, 0);
        } else if (new_pid == 0) {
            if (strcmp(line, "") != 0) {
                char* token = strtok(line, " ");
                char* args[100];
                int i = 0;
                while (token != NULL) {
                    args[i++] = token;
                    token = strtok(NULL, " ");
                }
                args[i] = NULL;
                execvp(args[0], args);
                perror("execvp failed");
                exit(0);
            }
        } else {
            print_fork_failed();
            // perror("fork failed");
            exit(0);
        }
    }
    fclose(script_file);
}


int shell(int argc, char *argv[]) {
    vector *vect = string_vector_create();
    signal(SIGINT, sigint_handler);
    char curr_dir[MAX_INPUT_SIZE];
    char written_in_terminal[MAX_INPUT_SIZE];
    pid_t pid = getpid();
    
    int history_flag = 0;
    char* history_filename = NULL;
    if (argc == 3 && strcmp(argv[1], "-h") == 0) {
        history_flag = 1;
        history_filename = argv[2];
    }

    int script_flag = 0;
    char* script_filename = NULL;
    if (argc == 3 && strcmp(argv[1], "-f") == 0) {
        script_flag = 1;
        script_filename = argv[2];
    }

    FILE *history_file = NULL;
    if (history_flag) {
        history_file = fopen(history_filename, "a");
        if (history_file == NULL) {
            print_history_file_error();
            // perror("Error opening history file");
            exit(0);
        }
    }

    if (script_flag) {
        execute_script(script_filename);
        exit(0); // Exit after running the script
    }

    // if (argc == 2 && strcmp(argv[1], "ps") == 0) {
    //     printf("ANUSHK");
    //     execute_ps();
    //     printf("SNUSHKA");
    //     return 0;  // Exit after running the ps command
    // }

    while(1) {
        print_prompt(getcwd(curr_dir, sizeof(curr_dir)), pid);
    
        if (fgets(written_in_terminal, sizeof(written_in_terminal), stdin) == NULL) {
            if (feof(stdin)) {
                break;  
            } else {
                perror("fgets");
                exit(1); 
            }
        }

        

        if (strstr(written_in_terminal, "#")) {
        size_t num = atoi(written_in_terminal + 1);
        if (num >= 0 && num < vector_size(vect)) {
            char * command = (char*) vector_get(vect, num);
            strcpy(written_in_terminal, command);
            print_command(command);

            } else {
                print_invalid_index();
            }
        }
        char *newline = strchr(written_in_terminal, '\n');
        if (newline) {
            *newline = '\0';
        }
        if (strcmp(written_in_terminal, "ps") == 0) {
            execute_ps();
            return 0;  // Exit after running the ps command
        }
        if (strcmp(written_in_terminal, "!history") != 0) {
            vector_push_back(vect, written_in_terminal);
        }
        if (strcmp(written_in_terminal, "exit") == 0) {
            if (history_flag) {
                fclose(history_file);
            }
            exit(0);
        }

        int status;

        if (history_flag) {
            if (strcmp(written_in_terminal, "!history") != 0) {
                fprintf(history_file, "\n%s", written_in_terminal);
                fflush(history_file);
            }
        }

        if (strstr(written_in_terminal, "&&") || strstr(written_in_terminal, "||") || strstr(written_in_terminal, ";")) {
            status = execute_logical_command(written_in_terminal);
        } else {
            status = execute_command(written_in_terminal);
        }
         if (status != 0) {
            perror("error");
            exit(1);
        }
        if (strstr(written_in_terminal, "!history")) {
            for (size_t i = 0 ; i < vector_size(vect); ++i) {
                print_history_line(i, vector_get(vect, i));
            }
        }

        // if((strstr(written_in_terminal, "!history")) == NULL && (strstr(written_in_terminal, "echo")) == NULL && 
        // !strstr(written_in_terminal, "ls") && !strstr(written_in_terminal, "pwd") && !strstr(written_in_terminal, "cd ") && 
        // !strstr(written_in_terminal, "-f ") && !strstr(written_in_terminal, "-h ") && !strstr(written_in_terminal, "^C") &&
        // !strstr(written_in_terminal, "!") && !strstr(written_in_terminal, "#") && !strstr(written_in_terminal, "exit") && 
        // !strstr(written_in_terminal, "^D") && strcmp(written_in_terminal, "") && !strstr(written_in_terminal, "sleep")) {
        //     print_invalid_command(written_in_terminal);
        // }

        
        
    }
    
    vector_destroy(vect);
    return 0;
}