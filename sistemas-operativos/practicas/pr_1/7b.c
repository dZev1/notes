#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void system_2(const char *arg) {
  if (arg == NULL)
    return;

  pid_t child = fork();

  if (child == 0) {
    execl("/bin/sh", "sh", "-c", arg, (char *)NULL);
    exit(EXIT_FAILURE);
  } else {
    wait(NULL);
  }
}
