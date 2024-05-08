#include <stdio.h>
#include <unistd.h>

#define READ 0
#define WRITE 1

int main() {
  int pipe1[2];
  pipe(pipe1);

  int x;

  int pid = fork();
  if (pid == -1) {
    perror("fork() failed");
    return 1;
  }

  if (pid == 0) {
    close(pipe1[READ]);
    x = 10;
    printf("Process %d sleeps for 5s.\n", getpid());
    sleep(5);
    printf("Process %d writes to pipe1.\n", getpid());
    write(pipe1[WRITE], &x, sizeof(x));
    close(pipe1[WRITE]);
  } else {
    close(pipe1[WRITE]);
    printf("Process %d is waiting for read from pipe1.\n", getpid());
    read(pipe1[READ], &x, sizeof(x));
    printf("Process %d has read %d from pipe1.\n", getpid(), x);
    close(pipe1[READ]);
  }
  printf("Process %d ends.\n", getpid());
}
