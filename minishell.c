/*********************************************************************
   Program  : miniShell                   Version    : 1.3
 --------------------------------------------------------------------
   skeleton code for linix/unix/minix command line interpreter
 --------------------------------------------------------------------
   File         : minishell.c
   Compiler/System   : gcc/linux

********************************************************************/

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define NV 20            /* max number of command tokens */
#define NL 100           /* input buffer size */
char line[NL];           /* command input buffer */

void prompt(void) {
    fprintf(stdout, "\n msh> ");
    fflush(stdout);
}

void handle_background_jobs() {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("[Process %d done]\n", pid);
    }
}

int main(int argk, char *argv[], char *envp[])
{
    int frkRtnVal;   /* value returned by fork sys call */
    int wpid;        /* value returned by wait */
    char *v[NV];     /* array of pointers to command line tokens */
    char *sep = " \t\n";  /* command line token separators */
    int i;           /* parse index */
    int bg;          /* flag for background execution */

    signal(SIGCHLD, handle_background_jobs);

    while (1) {           /* do Forever */
        prompt();
        if (fgets(line, NL, stdin) == NULL) {
            if (feof(stdin)) {
                printf("EOF pid %d feof %d ferror %d\n", getpid(), feof(stdin), ferror(stdin));
                exit(0);
            }
            continue;
        }
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\000')
            continue;         /* to prompt */

        v[0] = strtok(line, sep);
        for (i = 1; i < NV; i++) {
            v[i] = strtok(NULL, sep);
            if (v[i] == NULL)
                break;
        }

        /* assert i is number of tokens + 1 */

        /* check if command is to be executed in background */
        bg = 0;
        if (i > 1 && strcmp(v[i - 1], "&") == 0) {
            bg = 1;
            v[i - 1] = NULL;  /* remove & from command */
        }

        /* handle the cd command */
        if (strcmp(v[0], "cd") == 0) {
            if (v[1] == NULL) {
                if (chdir(getenv("HOME")) != 0) {
                    perror("cd");
                }
            } else {
                if (chdir(v[1]) != 0) {
                    perror("cd");
                }
            }
            continue;
        }

        /* fork a child process to exec the command in v[0] */

        switch (frkRtnVal = fork()) {
        case -1:           /* fork returns error to parent process */
            perror("fork");
            break;
        case 0:            /* code executed only by child process */
            execvp(v[0], v);
            perror("execvp");
            exit(EXIT_FAILURE);  /* terminate child process if exec fails */
        default:           /* code executed only by parent process */
            if (!bg) {
                wpid = waitpid(frkRtnVal, NULL, 0);
                if (wpid == -1) {
                    perror("waitpid");
                }
                printf("%s done \n", v[0]);
            } else {
                printf("[Running background job]\n");
            }
            break;
        }               /* switch */
    }                   /* while */
}                   /* main */
