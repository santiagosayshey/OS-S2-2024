/*********************************************************************
   Program  : miniShell                   Version    : 1.3
 --------------------------------------------------------------------
   skeleton code for linix/unix/minix command line interpreter
 --------------------------------------------------------------------
   File            : minishell.c
   Compiler/System : gcc/linux
********************************************************************/

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define NV 20           /* max number of command tokens */
#define NL 100          /* input buffer size */
#define MAX_BG_PROCESSES 10  /* New: maximum number of background processes */

char line[NL];  /* command input buffer */

/* New: Structure to keep track of background processes */
struct bg_process {
    pid_t pid;
    char command[NL];
    int finished;
};

/* New: Array to store background processes */
struct bg_process bg_processes[MAX_BG_PROCESSES];
int bg_count = 0;

/* New: Function to check and print finished background processes */
void check_and_print_finished_processes() {
    for (int i = 0; i < bg_count; i++) {
        if (bg_processes[i].pid != 0) {
            int status;
            pid_t result = waitpid(bg_processes[i].pid, &status, WNOHANG);
            if (result == bg_processes[i].pid) {
                fprintf(stdout, "[%d]+ Done                    %s\n", i+1, bg_processes[i].command);
                fflush(stdout);
                bg_processes[i].pid = 0;
            } else if (result == -1) {
                perror("waitpid");
            }
        }
    }
}

int main() {
    int frkRtnVal;  /* value returned by fork sys call */
    int wpid;       /* value returned by wait */
    char *v[NV];    /* array of pointers to command line tokens */
    char *sep = " \t\n";  /* command line token separators */
    int i;          /* parse index */

    while (1) {  /* do Forever */
        /* New: Use if statement to check for EOF */
        if (fgets(line, NL, stdin) == NULL) {
            if (feof(stdin)) {
                exit(0);
            }
        }

        v[0] = strtok(line, sep);
        for (i = 1; i < NV; i++) {
            v[i] = strtok(NULL, sep);
            if (v[i] == NULL)
                break;
        }

        /* New: Check for empty input and process finished background tasks */
        if (v[0] == NULL) {
            check_and_print_finished_processes();
            continue;
        }

        /* New: Handle 'cd' command */
        if (strcmp(v[0], "cd") == 0) {
            if (v[1] == NULL) {
                if (chdir(getenv("HOME")) != 0) {
                    perror("chdir");
                }
            } else {
                if (chdir(v[1]) != 0) {
                    perror("chdir");
                }
            }
            check_and_print_finished_processes();
            continue;
        }

        /* New: Check for background process */
        int is_background = 0;
        if (i > 1 && strcmp(v[i-1], "&") == 0) {
            is_background = 1;
            v[i-1] = NULL;
        }

        switch (frkRtnVal = fork()) {
            case -1:  /* fork returns error to parent process */
                perror("fork");
                break;
            case 0:  /* code executed only by child process */
                execvp(v[0], v);
                perror("execvp");
                exit(1);  /* New: Properly terminate child if exec fails */
            default:  /* code executed only by parent process */
                if (is_background) {
                    bg_count++;
                    fprintf(stdout, "[%d] %d\n", bg_count, frkRtnVal);
                    fflush(stdout);
                    bg_processes[bg_count-1].pid = frkRtnVal;
                   
                    /* New: Store the full command including arguments */
                    strcpy(bg_processes[bg_count-1].command, "");
                    for (int j = 0; v[j] != NULL; j++) {
                        strcat(bg_processes[bg_count-1].command, v[j]);
                        if (v[j+1] != NULL) strcat(bg_processes[bg_count-1].command, " ");
                    }
                   
                    bg_processes[bg_count-1].finished = 0;
                } else {
                    wpid = waitpid(frkRtnVal, NULL, 0);
                    if (wpid == -1) {
                        perror("waitpid");
                    }
                    check_and_print_finished_processes();
                }
                break;
        }
    }
    return 0;
}