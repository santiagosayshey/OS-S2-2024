#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define NV 20			/* max number of command tokens */
#define NL 100			/* input buffer size */
char            line[NL];	/* command input buffer */

/*
	shell prompt
 */

void prompt(void)
{
  fprintf(stdout, "\n msh> ");
  fflush(stdout);
}

void handle_sigchld(int sig) {
  int status;
  pid_t pid;
  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    printf("\nBackground process %d done\n", pid);
  }
}

int main(int argk, char *argv[], char *envp[])
/* argk - number of arguments */
/* argv - argument vector from command line */
/* envp - environment pointer */

{
  int             frkRtnVal;	/* value returned by fork sys call */
  int             wpid;		/* value returned by wait */
  char           *v[NV];	/* array of pointers to command line tokens */
  char           *sep = " \t\n";/* command line token separators    */
  int             i;		/* parse index */
  int             background;   /* flag for background processes */

  signal(SIGCHLD, handle_sigchld);

  /* prompt for and process one command line at a time  */

  while (1) {			/* do Forever */
    prompt();
    fgets(line, NL, stdin);
    fflush(stdin);

    if (feof(stdin)) {		/* non-zero on EOF  */

      fprintf(stderr, "EOF pid %d feof %d ferror %d\n", getpid(),
	      feof(stdin), ferror(stdin));
      exit(0);
    }
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\000')
      continue;			/* to prompt */

    v[0] = strtok(line, sep);
    for (i = 1; i < NV; i++) {
      v[i] = strtok(NULL, sep);
      if (v[i] == NULL)
	break;
    }
    /* assert i is number of tokens + 1 */

    background = 0;
    if (i > 1 && strcmp(v[i-1], "&") == 0) {
      background = 1;
      v[i-1] = NULL;
    }

    /* fork a child process to exec the command in v[0] */

    switch (frkRtnVal = fork()) {
    case -1:			/* fork returns error to parent process */
      {
        perror("fork");
	break;
      }
    case 0:			/* code executed only by child process */
      {
	execvp(v[0], v);
	perror("execvp");
        exit(1);
      }
    default:			/* code executed only by parent process */
      {
        if (!background) {
	  wpid = wait(0);
	  printf("%s done \n", v[0]);
	} else {
          printf("[%d] %s\n", frkRtnVal, v[0]);
        }
	break;
      }
    }				/* switch */
  }				/* while */

  return 0;  // Added return statement
}				/* main */