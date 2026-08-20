#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

volatile sig_atomic_t sig_received = false;

void sig_handler() {
    sig_received = true;
}

int main() {
  struct sigaction sa;
  sigset_t blocked_mask, og_mask;

  sa.sa_handler = sig_handler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGUSR1, &sa, NULL);
  sigaction(SIGUSR2, &sa, NULL);

  sigemptyset(&blocked_mask);
  sigaddset(&blocked_mask, SIGUSR1);
  sigaddset(&blocked_mask, SIGUSR2);

  sigprocmask(SIG_BLOCK, &blocked_mask, &og_mask);

  pid_t child = fork();

  if (child < 0) {
    printf("Hubo un error, reintente de nuevo.\n");
    exit(EXIT_FAILURE);
  } else if (child == 0) {
    for (;;) {
      while (!sig_received) {
        sigsuspend(&og_mask);
      }
      sig_received = false;

      printf("[CHILD (pid=%d)] Pong\n", getpid());
      kill(getppid(), SIGUSR2);
    }
  } else {
    char character;
    bool _continue = true;

    while (_continue) {
      for (int i = 0; i < 3; i++) {
        printf("[PARENT (pid=%d)] Ping\n", getpid());
        kill(child, SIGUSR1);

        while (!sig_received) {
          sigsuspend(&og_mask);
        }
        sig_received = false;
      }

      printf("Continue? [Y/n]: ");
      scanf(" %c", &character);
      if (character == 'n') {
          _continue = false;
      }
    }
  }

  kill(child, SIGTERM);
  wait(NULL);
  exit(EXIT_SUCCESS);
}
