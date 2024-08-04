#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#define NV 20     /* max number of command tokens */
#define NL 100    /* input buffer size */

char line[NL];    /* command input buffer */

int is_background(char **v, int count) {
    if (count > 0 && strcmp(v[count-1], "&") == 0) {
        v[count-1] = NULL;
        return 1;
    }
    return 0;
}

void sigchld_handler(int signo) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            printf("Background process %d finished with exit status %d\n", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Background process %d terminated by signal %d\n", pid, WTERMSIG(status));
        }
    }
    if (pid < 0 && errno != ECHILD) {
        perror("waitpid");
    }
}

void change_directory(char **args) {
    if (args[1] == NULL) {
        char *home = getenv("HOME");
        if (home == NULL) {
            fprintf(stderr, "HOME environment variable not set\n");
            return;
        }
        if (chdir(home) != 0) {
            perror("cd");
        }
    } else {
        if (chdir(args[1]) != 0) {
            perror("cd");
        }
    }
}

void prompt(void) {
    if (fprintf(stdout, "\n msh> ") < 0) {
        perror("fprintf");
    }
    if (fflush(stdout) != 0) {
        perror("fflush");
    }
}

int main(int argk, char *argv[], char *envp[]) {
    int frkRtnVal;
    char *v[NV];
    char *sep = " \t\n";
    int i;
    int bg;

    if (signal(SIGCHLD, sigchld_handler) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    while (1) {
        prompt();
        if (fgets(line, NL, stdin) == NULL) {
            if (feof(stdin)) {
                if (fprintf(stderr, "EOF pid %d feof %d ferror %d\n", getpid(),
                    feof(stdin), ferror(stdin)) < 0) {
                    perror("fprintf");
                }
                exit(0);
            }
            if (ferror(stdin)) {
                perror("fgets");
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

        if (strcmp(v[0], "cd") == 0) {
            change_directory(v);
            continue;
        }

        bg = is_background(v, i);

        switch (frkRtnVal = fork()) {
        case -1:
            perror("fork");
            break;
        case 0:
            execvp(v[0], v);
            perror("execvp");
            _exit(EXIT_FAILURE);  // Terminate child process if exec fails
        default:
            if (!bg) {
                int status;
                if (waitpid(frkRtnVal, &status, 0) == -1) {
                    perror("waitpid");
                } else {
                    if (WIFEXITED(status)) {
                        printf("%s done with exit status %d\n", v[0], WEXITSTATUS(status));
                    } else if (WIFSIGNALED(status)) {
                        printf("%s terminated by signal %d\n", v[0], WTERMSIG(status));
                    }
                }
            } else {
                if (printf("Started background process %d\n", frkRtnVal) < 0) {
                    perror("printf");
                }
            }
            break;
        }
    }
}