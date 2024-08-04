#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void print_even(int num) {
    for (int i=0; i<num+1; i++) {
        if (!(i % 2)) {
            printf("%d\n", i);
            sleep(5);
        }
    }
}

void handle_sigint() {
    printf("Yeah!");
}

void handle_sighup() {
    printf("Ouch!");
}

int main(int argc, char **argv) {
    signal(SIGINT, handle_sigint);
    signal(SIGHUP, handle_sighup);

    if (argc > 1) {
        int num = atoi(argv[1]);
        print_even(num);
    } else {
        printf("No argument provided\n");
    }
    return 0;
}
