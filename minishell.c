/*********************************************************************
   Program  : miniShell                   Version    : 1.3
 --------------------------------------------------------------------
   skeleton code for linix/unix/minix command line interpreter
 --------------------------------------------------------------------
   File			: minishell.c
   Compiler/System	: gcc/linux

********************************************************************/

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


  /* prompt for and process one command line at a time  */

  while (1) {			/* do Forever */
    if (fgets(line, NL, stdin) == NULL) {
      if (feof(stdin)) {		/* non-zero on EOF  */
        exit(0);
      }
    }
    fflush(stdin);

    if (line[0] == '#' || line[0] == '\n' || line[0] == '\000')
      continue;			/* to prompt */

    v[0] = strtok(line, sep);
    for (i = 1; i < NV; i++) {
      v[i] = strtok(NULL, sep);
      if (v[i] == NULL)
	break;
    }
    /* assert i is number of tokens + 1 */

    // Handle cd command
    if (v[0] && strcmp(v[0], "cd") == 0) {
      if (v[1]) {
        if (chdir(v[1]) != 0) {
          perror("cd");
        }
      } else {
        char *home = getenv("HOME");
        if (home) {
          if (chdir(home) != 0) {
            perror("cd");
          }
        } else {
          fprintf(stderr, "cd: HOME not set\n");
        }
      }
      continue;  // Skip to next iteration after handling cd
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
	perror(v[0]);
        exit(1);
      }
    default:			/* code executed only by parent process */
      {
	wpid = wait(NULL);
	break;
      }
    }				/* switch */
  }				/* while */
}				/* main */