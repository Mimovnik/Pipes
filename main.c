#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define READ 0
#define WRITE 1
#define INPUT_BUFFER_SIZE 64

void strupper(char *str) {
  for (int i = 0; i < strlen(str); i++) {
    if (str[i] >= 'a' && str[i] <= 'z') {
      str[i] = toupper((unsigned char)str[i]);
    }
  }
}

int main() {
  int pipe1[2];
  pipe(pipe1);

  char message[INPUT_BUFFER_SIZE];

  int pid = fork();
  if (pid == -1) {
    perror("fork() failed");
    return 1;
  }

  if (pid != 0) {
    // Read and send the message
    close(pipe1[READ]);
    while (1) {
      puts("Enter a message (or q to quit):");
      scanf("%s", message);
      printf("Process  %d writes '%s' to pipe1.\n", getpid(), message);
      write(pipe1[WRITE], &message, sizeof(message));
      if (strcmp(message, "q") == 0) {
        printf("Process  %d quits.\n", getpid());
        close(pipe1[WRITE]);
        return 0;
      }
    }
  } else {
    // Transform to uppercase and send
    close(pipe1[WRITE]);
    while (1) {
      printf("Process %d is waiting for read from pipe1.\n", getpid());
      read(pipe1[READ], &message, sizeof(message));
      printf("Process %d has read '%s' from pipe1.\n", getpid(), message);
      if (strcmp(message, "q") == 0) {
        printf("Process  %d quits.\n", getpid());
        close(pipe1[READ]);
        return 0;
      }
      printf(
          "Process %d transforms '%s' to uppercase and sleeps for a while.\n",
          getpid(), message);
      strupper(message);
      sleep(5);
      printf("Process %d transformed a message to '%s'.\n", getpid(), message);
    }
  }
  printf("Process %d ends.\n", getpid());
}
