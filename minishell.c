#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define NV 20     /* max number of command tokens */
#define NL 100    /* input buffer size */

char line[NL];    /* command input buffer */

/* Function to check if command should run in background */
int is_background(char **v, int count) {
    if (count > 0 && strcmp(v[count-1], "&") == 0) {
        v[count-1] = NULL;  // Remove the '&'
        return 1;
    }
    return 0;
}

/* Signal handler for SIGCHLD */
void sigchld_handler(int signo) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("Background process %d finished\n", pid);
    }
}

/* shell prompt */
void prompt(void) {
    fprintf(stdout, "\n msh> ");
    fflush(stdout);
}

int main(int argk, char *argv[], char *envp[]) {
    int frkRtnVal;
    char *v[NV];
    char *sep = " \t\n";
    int i;
    int bg;

    /* Set up SIGCHLD handler */
    signal(SIGCHLD, sigchld_handler);

    while (1) {
        prompt();
        if (fgets(line, NL, stdin) == NULL) {
            if (feof(stdin)) {
                fprintf(stderr, "EOF pid %d feof %d ferror %d\n", getpid(),
                    feof(stdin), ferror(stdin));
                exit(0);
            }
            continue;
        }

        if (line[0] == '#' || line[0] == '\n' || line[0] == '\0')
            continue;

        v[0] = strtok(line, sep);
        for (i = 1; i < NV; i++) {
            v[i] = strtok(NULL, sep);
            if (v[i] == NULL)
                break;
        }

        bg = is_background(v, i);

        switch (frkRtnVal = fork()) {
        case -1:
            perror("fork");
            break;
        case 0:
            execvp(v[0], v);
            perror("execvp");
            exit(1);
        default:
            if (!bg) {
                waitpid(frkRtnVal, NULL, 0);
                printf("%s done \n", v[0]);
            } else {
                printf("Started background process %d\n", frkRtnVal);
            }
            break;
        }
    }
}