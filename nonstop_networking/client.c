//used chatGPT for debugging and initial structure

/**
 * nonstop_networking
 * CS 341 - Fall 2023
 */
#include "format.h"
#include <stdbool.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "common.h"

char **parse_args(int argc, char **argv);
verb check_args(char **args);
int connect_to_server(char **args);
void write_cmd(char **args, int socket, verb method);
void read_response(char **args, int socket, verb method);


int main(int argc, char **argv) {
    // Good luck!
    char **args = parse_args(argc, argv);
    if (args == NULL) {
        return 1;
    }
    verb method = check_args(args);
    int serverSocket = connect_to_server(args);
    if (serverSocket < 0) {
        free(args);
        return 1;
    }
    write_cmd(args, serverSocket, method);
    if (shutdown(serverSocket, SHUT_WR) != 0) {
        perror("shutdown()");
    }
    read_response(args, serverSocket, method);
    shutdown(serverSocket, SHUT_RD);
    close(serverSocket);
    free(args);
}

// void read_response(char **args, int socket, verb method) {
//   char *buffer = calloc(1,strlen("OK\n")+1);
//   size_t bytes_rd = read_from_socket(socket, buffer, strlen("OK\n"));
//   if (strcmp(buffer, "OK\n") == 0) {
//     fprintf(stdout, "%s", buffer);
//     if (method == DELETE || method == PUT) {
//       print_success();
//     } else if (method == GET) {
//       FILE *local = fopen(args[4], "a+");
//       if (!local) {
//         perror(NULL);
//         exit(1);
//       }
//       size_t size;
//       read_from_socket(socket, (char *)&size, sizeof(size_t));
//       size_t bytes_read = 0;
//       while (bytes_read < size+5) {
//         size_t size_hd = (size+5-bytes_read) > 1024 ? 1024 : (size+5-bytes_read);
//         char buffer_f[1024+1] = {0};
//         size_t rc = read_from_socket(socket, buffer_f, size_hd);
//         fwrite(buffer_f, 1, rc, local);
//         bytes_read += rc;
//         if (rc == 0)
//           break;
//       }
//       if (print_any_err(bytes_read, size)) exit(1);
//       fclose(local);
//     } else if (method == LIST) {
//       size_t size;
//       read_from_socket(socket, (char *)&size, sizeof(size_t));
//       char buffer_f[size+5+1];
//       memset(buffer_f, 0, size+5+1);
//       bytes_rd = read_from_socket(socket, buffer_f, size+5);
//       if (print_any_err(bytes_rd, size)) exit(1);
//       fprintf(stdout, "%zu%s", size, buffer_f);
//     }
//   } else {
//     buffer = realloc(buffer, strlen("ERROR\n")+1);
//     read_from_socket(socket, buffer+bytes_rd, strlen("ERROR\n")-bytes_rd);
//     if (strcmp(buffer, "ERROR\n") == 0) {
//       fprintf(stdout, "%s", buffer);
//       char err[20] = {0};
//       if (!read_from_socket(socket, err, 20))
//         print_connection_closed();
//       print_error_message(err);
//     } else {
//       print_invalid_response();
//     }
//   }
//   free(buffer);
// }

void read_response(char **arguments, int socket, verb operation) {
    char *response_buffer = calloc(1, strlen("OK\n") + 1);
    size_t bytes_received = read_from_socket(socket, response_buffer, strlen("OK\n"));

    if (strcmp(response_buffer, "OK\n") == 0) {
        fprintf(stdout, "%s", response_buffer);

        if (operation == DELETE || operation == PUT) {
            print_success();
        } else if (operation == GET) {
            FILE *local_file = fopen(arguments[4], "a+");
            if (!local_file) {
                perror(NULL);
                exit(1);
            }

            size_t file_size;
            read_from_socket(socket, (char *)&file_size, sizeof(size_t));

            size_t bytes_read = 0;
            while (bytes_read < file_size + 5) {
                size_t remaining = (file_size + 5 - bytes_read);
                if (remaining > 1024) {
                    remaining = 1024;
                }

                char data_buffer[1024 + 1] = {0};
                size_t read_count = read_from_socket(socket, data_buffer, remaining);
                fwrite(data_buffer, 1, read_count, local_file);
                bytes_read += read_count;
                if (read_count == 0) {
                    break;
                }
            }

            if (print_any_err(bytes_read, file_size)) {
                exit(1);
            }
            fclose(local_file);
        } else if (operation == LIST) {
            size_t size;
            read_from_socket(socket, (char *)&size, sizeof(size_t));
            char data_buffer[size + 5 + 1];
            memset(data_buffer, 0, size + 5 + 1);
            bytes_received = read_from_socket(socket, data_buffer, size + 5);
            if (print_any_err(bytes_received, size)) {
                exit(1);
            }
            fprintf(stdout, "%zu%s", size, data_buffer);
        }
    } else {
        response_buffer = realloc(response_buffer, strlen("ERROR\n") + 1);
        read_from_socket(socket, response_buffer + bytes_received, strlen("ERROR\n") - bytes_received);

        if (strcmp(response_buffer, "ERROR\n") == 0) {
            fprintf(stdout, "%s", response_buffer);
            char error_message[20] = {0};
            if (!read_from_socket(socket, error_message, 20)) {
                print_connection_closed();
            }
            print_error_message(error_message);
        } else {
            print_invalid_response();
        }
    }
    free(response_buffer);
}


// void write_cmd(char **args, int socket, verb method) {
//   char *msg;
//   if (method == LIST) {
//     msg = calloc(1, strlen(args[2])+2);
//     sprintf(msg, "%s\n", args[2]);
//   } else {
//     msg = calloc(1, strlen(args[2])+strlen(args[3])+3);
//     sprintf(msg, "%s %s\n", args[2], args[3]);
//   }
//   ssize_t len = strlen(msg);
//   if (write_to_socket(socket, msg, len) < len) {
//     print_connection_closed();
//     exit(1);
//   }
//   free(msg);

//   if (method == PUT) {
//     struct stat buf;
//     if(stat(args[4], &buf) == -1)
//       exit(1);
//     size_t size = buf.st_size;
//     write_to_socket(socket, (char*)&size, sizeof(size_t));
//     FILE *local = fopen(args[4], "r");
//     if (!local) {
//       fprintf(stdout, "local file open fail\n");
//       exit(1);
//     }
//     size_t bytes_write = 0;
//     while (bytes_write < size) {
//       ssize_t size_hd = (size-bytes_write) > 1024 ? 1024 : (size-bytes_write);
//       char buffer[size_hd+1];
//       fread(buffer, 1, size_hd, local);
//       if (write_to_socket(socket, buffer, size_hd) < size_hd) {
//         print_connection_closed();
//         exit(1);
//         }
//       bytes_write += size_hd;
//     }
//     fclose(local);
//   }
// }

void write_cmd(char **args, int socket, verb method) {
    if (method == LIST) {
        char *msg = calloc(1, strlen(args[2]) + 2);
        sprintf(msg, "%s\n", args[2]);
        ssize_t len = strlen(msg);
        if (write_to_socket(socket, msg, len) < len) {
            print_connection_closed();
            exit(1);
        }
        free(msg);
    } else {
        char *msg = calloc(1, strlen(args[2]) + strlen(args[3]) + 3);
        sprintf(msg, "%s %s\n", args[2], args[3]);
        ssize_t len = strlen(msg);
        if (write_to_socket(socket, msg, len) < len) {
            print_connection_closed();
            exit(1);
        }
        free(msg);

        if (method == PUT) {
            struct stat buf;
            if (stat(args[4], &buf) == -1) {
                exit(1);
            }

            size_t size = buf.st_size;
            if ((size_t) write_to_socket(socket, (char*)&size, sizeof(size_t)) < sizeof(size_t)) {
                print_connection_closed();
                exit(1);
            }

            FILE *local = fopen(args[4], "r");
            if (!local) {
                fprintf(stdout, "local file open fail\n");
                exit(1);
            }

            size_t bytes_write = 0;
            while (bytes_write < size) {
                size_t remaining = (size - bytes_write) > 1024 ? 1024 : (size - bytes_write);
                char buffer[remaining];
                fread(buffer, 1, remaining, local);
                if ((size_t)write_to_socket((size_t) socket, buffer, remaining) < remaining) {
                    print_connection_closed();
                    exit(1);
                }
                bytes_write += remaining;
            }
            fclose(local);
        }
    }
}


int connect_to_server(char **args) {
  struct addrinfo hints, *result;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  int s = getaddrinfo(args[0], args[1], &hints, &result);
  if (s != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    exit(1);
  }
  int sock_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (sock_fd == -1) {
    perror(NULL);
    exit(1);
  }
  int ok = connect(sock_fd, result->ai_addr, result->ai_addrlen);
  if (ok == -1) {
    perror(NULL);
    exit(1);
  }
  freeaddrinfo(result);
  return sock_fd;
}


// int connect_to_server(char **args, int* error_code) {
//     struct addrinfo hints, *result;
//     memset(&hints, 0, sizeof(hints));
//     hints.ai_family = AF_INET;
//     hints.ai_socktype = SOCK_STREAM;

//     int s = getaddrinfo(args[0], args[1], &hints, &result);
//     if (s != 0) {
//         fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
//         *error_code = 1; // Set error code
//         return -1; // Return an error indicator
//     }

//     int sock_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
//     if (sock_fd == -1) {
//         perror(NULL);
//         freeaddrinfo(result);
//         *error_code = 2; // Set error code
//         return -1; // Return an error indicator
//     }

//     int ok = connect(sock_fd, result->ai_addr, result->ai_addrlen);
//     if (ok == -1) {
//         perror(NULL);
//         close(sock_fd);
//         freeaddrinfo(result);
//         *error_code = 3; // Set error code
//         return -1; // Return an error indicator
//     }

//     freeaddrinfo(result);
//     *error_code = 0; // Set success code
//     return sock_fd;
// }

// typedef enum {
//     NO_ERROR,
//     ADDR_INFO_ERROR,
//     SOCKET_ERROR,
//     CONNECT_ERROR
// } ErrorCode;

// typedef struct {
//     int socket_fd;
//     ErrorCode error_code;
//     bool connection_successful;
// } ConnectionInfo;

// void handle_error(ErrorCode error_code) {
//     switch (error_code) {
//         case ADDR_INFO_ERROR:
//             fprintf(stderr, "getaddrinfo error\n");
//             break;
//         case SOCKET_ERROR:
//             perror("socket creation error");
//             break;
//         case CONNECT_ERROR:
//             perror("connection error");
//             break;
//         default:
//             break;
//     }
// }

// ConnectionInfo connect_to_server(char **args) {
//     ConnectionInfo connection;
//     struct addrinfo hints, *result;
//     memset(&hints, 0, sizeof(hints));
//     hints.ai_family = AF_INET;
//     hints.ai_socktype = SOCK_STREAM;

//     int s = getaddrinfo(args[0], args[1], &hints, &result);
//     if (s != 0) {
//         connection.error_code = ADDR_INFO_ERROR;
//         connection.connection_successful = false;
//         handle_error(connection.error_code);
//         return connection;
//     }

//     int sock_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
//     if (sock_fd == -1) {
//         freeaddrinfo(result);
//         connection.error_code = SOCKET_ERROR;
//         connection.connection_successful = false;
//         handle_error(connection.error_code);
//         return connection;
//     }

//     int ok = connect(sock_fd, result->ai_addr, result->ai_addrlen);
//     if (ok == -1) {
//         close(sock_fd);
//         freeaddrinfo(result);
//         connection.error_code = CONNECT_ERROR;
//         connection.connection_successful = false;
//         handle_error(connection.error_code);
//         return connection;
//     }

//     connection.socket_fd = sock_fd;
//     connection.error_code = NO_ERROR;
//     connection.connection_successful = true;

//     freeaddrinfo(result);
//     return connection;
// }



/**
 * Given commandline argc and argv, parses argv.
 *
 * argc argc from main()
 * argv argv from main()
 *
 * Returns char* array in form of {host, port, method, remote, local, NULL}
 * where `method` is ALL CAPS
 */
char **parse_args(int argc, char **argv) {
    if (argc < 3) {
        return NULL;
    }

    char *host = strtok(argv[1], ":");
    char *port = strtok(NULL, ":");
    if (port == NULL) {
        return NULL;
    }

    char **args = calloc(1, 6 * sizeof(char *));
    args[0] = host;
    args[1] = port;
    args[2] = argv[2];
    char *temp = args[2];
    while (*temp) {
        *temp = toupper((unsigned char)*temp);
        temp++;
    }
    if (argc > 3) {
        args[3] = argv[3];
    }
    if (argc > 4) {
        args[4] = argv[4];
    }

    return args;
}

/**
 * Validates args to program.  If `args` are not valid, help information for the
 * program is printed.
 *
 * args     arguments to parse
 *
 * Returns a verb which corresponds to the request method
 */
verb check_args(char **args) {
    if (args == NULL) {
        print_client_usage();
        exit(1);
    }

    char *command = args[2];

    if (strcmp(command, "LIST") == 0) {
        return LIST;
    }

    if (strcmp(command, "GET") == 0) {
        if (args[3] != NULL && args[4] != NULL) {
            return GET;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "DELETE") == 0) {
        if (args[3] != NULL) {
            return DELETE;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "PUT") == 0) {
        if (args[3] == NULL || args[4] == NULL) {
            print_client_help();
            exit(1);
        }
        return PUT;
    }

    // Not a valid Method
    print_client_help();
    exit(1);
}
