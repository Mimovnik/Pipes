#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ 0
#define WRITE 1
#define INPUT_BUFFER_SIZE 64
#define CONTINUE 1
#define EXIT 0

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
  int pipe2[2];
  pipe(pipe2);

  char message[INPUT_BUFFER_SIZE];

  int pid = fork();
  if (pid == -1) {
    perror("fork() failed");
    exit(1);
  }

  if (pid != 0) {
    // INPUT- reads and sends the message
    close(pipe1[READ]);
    close(pipe2[READ]);
    close(pipe2[WRITE]);
    while (1) {

      int status = CONTINUE;

      puts("INPUT: Enter a message (or q to quit):");
      scanf("%s", message);

      if (strcmp(message, "q") == 0) {
        status = EXIT;
      }

      write(pipe1[WRITE], &status, sizeof(status));

      if (status == EXIT) {
        puts("INPUT: quits.\n");
        close(pipe1[WRITE]);
        waitpid(pid, NULL, 0);
        exit(0);
      }

      printf("INPUT: writes '%s' to pipe1.\n", message);
      write(pipe1[WRITE], &message, sizeof(message));
    }
  } else {
    int pid2 = fork();
    if (pid2 == -1) {
      perror("fork() failed");
      exit(1);
    }

    if (pid2 != 0) {
      // TRANSFORM- transforms to uppercase and sends
      close(pipe1[WRITE]);
      close(pipe2[READ]);
      while (1) {
        int status;
        read(pipe1[READ], &status, sizeof(status));

        if (status == EXIT) {
          puts("TRANSFORM: quits.\n");
          write(pipe2[WRITE], &status, sizeof(status));
          close(pipe1[READ]);
          close(pipe2[WRITE]);
          waitpid(pid2, NULL, 0);
          exit(0);
        }

        read(pipe1[READ], &message, sizeof(message));
        printf(
            "TRANSFORM: transforms '%s' to uppercase and sleeps for a while.\n",
            message);
        strupper(message);
        sleep(5);

        write(pipe2[WRITE], &status, sizeof(status));

        printf("TRANSFORM: writes transformed '%s' to pipe2.\n", message);
        write(pipe2[WRITE], &message, sizeof(message));
      }
    } else {
      // PRINT- prints transformed message
      close(pipe1[READ]);
      close(pipe2[WRITE]);
      close(pipe2[WRITE]);
      while (1) {
        int status;
        read(pipe2[READ], &status, sizeof(status));
        if (status == EXIT) {
          puts("PRINT: quits.\n");
          close(pipe1[READ]);
          close(pipe2[WRITE]);
          exit(0);
        }
        read(pipe2[READ], &message, sizeof(message));
        printf("PRINT: has read '%s' from pipe2.\n", message);
      }
    }
  }
}
