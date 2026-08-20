#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    printf("[ABRAHAM] Hola, nací y soy Abraham\n");
    pid_t homer_pid = fork();
    if (homer_pid == 0) {
        printf("[HOMERO] Soy Homero, pero chino.\n");

        pid_t bart_pid = fork();
        if (bart_pid == 0) {
            printf("[BART] Ay Caramba.\n");
            exit(EXIT_SUCCESS);
        }

        pid_t lisa_pid = fork();
        if (lisa_pid == 0) {
            printf("[LISA] Soy Lisa y soy vegana.\n");
            exit(EXIT_SUCCESS);
        }

        pid_t maggie_pid = fork();
        if (maggie_pid == 0) {
            printf("[MAGGIE] *ruido de chupete*.\n");
            exit(EXIT_SUCCESS);
        }

        wait(NULL);
        wait(NULL);
        wait(NULL);

        exit(EXIT_SUCCESS);
    }

    wait(NULL);
    return EXIT_SUCCESS;
}
