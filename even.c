#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void print_even(int num) {
    int current = 0;
    int toPrint = 0;

    while (current < num) {
        printf("%d\n", toPrint); 
        toPrint += 2;
        current++;
        sleep(5);
    }
}

void handle_sigint() {
    printf("Yeah!\n");
    signal(SIGINT, handle_sigint); 
}

void handle_sighup() {
    printf("Ouch!\n");
    signal(SIGHUP, handle_sighup);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number>\n", argv[0]);
        return 1;
    }

    int num = atoi(argv[1]);

    signal(SIGINT, handle_sigint);
    signal(SIGHUP, handle_sighup);

    print_even(num);

    return 0;
}