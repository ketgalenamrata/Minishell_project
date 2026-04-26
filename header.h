#ifndef MAIN_H
#define MAIN_H

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[1;3;32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
// ----------------- Header files ------------------- //
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
 
// ------------------- MACROS -------------------------- //
#define BUILTIN		1
#define EXTERNAL	2
#define NO_COMMAND  3


// ----------------- Structure declaration --------------- //
typedef struct stoped_jobs
{
    char command[50];
    int pid;
}s_jobs;


// ------------------ Function Declarations ------------------ //
void scan_input(char *prompt);
char *get_command();
int check_command_type(char *command);
void execute_internal_commands(char *cmd);
void extract_external_commands(char **external_commands);
void n_pipe(int pipe_count, char *argv[]);
void extract_external_cmds_from_input_string(int *pipe_count,char** execute);
void my_handler(int signum);

// ---------------- Array job operations ---------------- //
void insert_jobs_to_array();
void print_jobs();
void delete_job();

#endif