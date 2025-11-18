/**
 * @file ysh.c
 * @brief A simple shell implementation for Operating Systems Assignment 1.
 * Supports basic command execution, pipes, and sequential execution.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

#define BUF_SIZE 128    // Max length of user input
#define MAX_ARGS 8      // Max number of arguments per process

/**
 * @brief Reads input from stdin, removing the trailing newline.
 * @param buf Buffer to store the input string.
 * @return Pointer to the buffer, or NULL on EOF/Error.
 */
char* readcmd(char *buf) {
    fprintf(stdout, "$ ");
    memset(buf, 0, BUF_SIZE);
    
    // Check for EOF or error to prevent undefined behavior
    if (fgets(buf, BUF_SIZE, stdin) == NULL) {
        return NULL;
    }

    // Chop the tailing \n if it exists
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') {
        buf[len-1] = 0;
    }

    return buf;
}

/**
 * @brief Trims leading and trailing whitespace from a string.
 * @param str The string to trim.
 * @return Pointer to the trimmed string.
 */
char* trim_whitespace(char *str) {
    char *end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end+1) = 0;

    return str;
}

/**
 * @brief Parses a command string into arguments array.
 * @param cmd Command string to parse.
 * @param argv Array to store pointers to arguments.
 * @return Number of arguments.
 */
int parse_args(char *cmd, char *argv[]) {
    int argc = 0;
    char *token;
    char *rest = cmd;

    while ((token = strsep(&rest, " ")) != NULL && argc < MAX_ARGS) {
        if (strlen(token) == 0) continue; // Skip empty tokens caused by multiple spaces
        argv[argc++] = token;
    }
    argv[argc] = NULL; // Null-terminate the array
    return argc;
}

/**
 * @brief Executes a single command (internal or external).
 * Handles "cd" and "exit" built-ins directly.
 * Forks and execs for external commands.
 * @param cmd The command string.
 * @return 0 on success, -1 if exit command is issued.
 */
int execute_single_command(char *cmd) {
    char *argv[MAX_ARGS + 1];
    
    cmd = trim_whitespace(cmd);
    if (strlen(cmd) == 0) return 0;

    // Check for 'exit' before parsing arguments to handle it cleanly
    if (strncmp(cmd, "exit", 4) == 0 && (cmd[4] == '\0' || isspace(cmd[4]))) {
        return -1;
    }

    // Check for 'cd' built-in
    if (strncmp(cmd, "cd ", 3) == 0) {
        char *path = trim_whitespace(cmd + 3);
        if (chdir(path) < 0) {
            fprintf(stderr, "Cannot cd %s\n", path);
        }
        return 0;
    }
    
    // It's an external command
    int argc = parse_args(cmd, argv);
    if (argc == 0) return 0;

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execvp(argv[0], argv);
        fprintf(stderr, "Command not found: %s\n", argv[0]);
        exit(1);
    } else if (pid > 0) {
        // Parent process waits
        wait(NULL);
    } else {
        perror("fork failed");
    }

    return 0;
}

/**
 * @brief Executes commands connected by pipes.
 * @param cmd The command string containing pipes.
 * @return 0 on success, -1 on exit.
 */
int execute_piped_commands(char *cmd) {
    char *commands[MAX_ARGS]; // Reuse MAX_ARGS limit for max pipe segments
    int num_cmds = 0;
    char *token;
    char *rest = cmd;

    // Split by pipe symbol '|'
    while ((token = strsep(&rest, "|")) != NULL && num_cmds < MAX_ARGS) {
        commands[num_cmds++] = token;
    }

    // If no pipe, execute as single command
    if (num_cmds == 1) {
        return execute_single_command(commands[0]);
    }

    // Pipe handling
    int pipe_fds[2 * (num_cmds - 1)];
    
    // Create all necessary pipes
    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipe_fds + i * 2) < 0) {
            perror("pipe failed");
            return 0;
        }
    }

    int i;
    for (i = 0; i < num_cmds; i++) {
        char *sub_cmd = trim_whitespace(commands[i]);
        char *argv[MAX_ARGS + 1];
        parse_args(sub_cmd, argv);

        if (argv[0] == NULL) continue; // Skip empty commands

        pid_t pid = fork();
        if (pid == 0) {
            // Child process configuration
            
            // If not the first command, redirect stdin from previous pipe
            if (i > 0) {
                dup2(pipe_fds[(i - 1) * 2], 0);
            }

            // If not the last command, redirect stdout to next pipe
            if (i < num_cmds - 1) {
                dup2(pipe_fds[i * 2 + 1], 1);
            }

            // Close all pipe fds in child
            for (int j = 0; j < 2 * (num_cmds - 1); j++) {
                close(pipe_fds[j]);
            }

            execvp(argv[0], argv);
            fprintf(stderr, "Command not found: %s\n", argv[0]);
            exit(1);
        } else if (pid < 0) {
            perror("fork failed");
        }
    }

    // Parent closes all pipes
    for (int j = 0; j < 2 * (num_cmds - 1); j++) {
        close(pipe_fds[j]);
    }

    // Parent waits for all children
    for (int j = 0; j < num_cmds; j++) {
        wait(NULL);
    }

    return 0;
}

/**
 * @brief Main command runner. Handles semicolon separated commands.
 * @param cmd The raw input string.
 * @return 0 to continue, -1 to exit.
 */
int runcmd(char *cmd) {
    char *token;
    char *rest = cmd;
    
    // Handle sequential execution (semicolons)
    while ((token = strsep(&rest, ";")) != NULL) {
        // Process piped commands (or single command if no pipe) within the segment
        if (execute_piped_commands(token) == -1) {
            return -1; // Exit signal received
        }
    }

    return 0;
}

int main(void) {
    char *cmd = 0;
    char buf[BUF_SIZE];

    /* * NOTE: The original skeleton code opened a "console" file here. 
     * This is specific to xv6 and not appropriate for a standard Linux shell assignment.
     * It has been removed to prevent creating artifacts in the local filesystem.
     */

    fprintf(stdout, "EEE3535 Operating Systems: starting ysh\n");

    // Read and run input commands.
    while((cmd = readcmd(buf))) {
        // Run the input commands.
        if(*cmd && runcmd(cmd) == -1) { break; }
    }

    return 0;
}