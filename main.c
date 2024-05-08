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

// reads and sends the message
void input(int pipe1[2], int pipe2[2], int pid) {
  close(pipe1[READ]);
  close(pipe2[READ]);
  close(pipe2[WRITE]);
  while (1) {
    int status = CONTINUE;

    char message[INPUT_BUFFER_SIZE];
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
}

// transforms to uppercase and sends
void transform(int pipe1[2], int pipe2[2], int pid) {
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
      waitpid(pid, NULL, 0);
      exit(0);
    }

    char message[INPUT_BUFFER_SIZE];
    read(pipe1[READ], &message, sizeof(message));
    printf("TRANSFORM: transforms '%s' to uppercase and sleeps for a while.\n",
           message);
    strupper(message);
    sleep(5);

    write(pipe2[WRITE], &status, sizeof(status));

    printf("TRANSFORM: writes transformed '%s' to pipe2.\n", message);
    write(pipe2[WRITE], &message, sizeof(message));
  }
}

// prints transformed message
void display(int pipe1[2], int pipe2[2]) {
  close(pipe1[READ]);
  close(pipe2[WRITE]);
  close(pipe2[WRITE]);
  while (1) {
    int status;
    read(pipe2[READ], &status, sizeof(status));
    if (status == EXIT) {
      puts("DISPLAY: quits.\n");
      close(pipe1[READ]);
      close(pipe2[WRITE]);
      exit(0);
    }
    char message[INPUT_BUFFER_SIZE];
    read(pipe2[READ], &message, sizeof(message));
    printf("DISPLAY: has read '%s' from pipe2.\n", message);
  }
}

int main() {
  int pipe1[2];
  pipe(pipe1);
  int pipe2[2];
  pipe(pipe2);

  int pid = fork();
  if (pid == -1) {
    perror("fork() failed");
    exit(1);
  }

  if (pid != 0) {
    input(pipe1, pipe2, pid);
  } else {
    int pid2 = fork();
    if (pid2 == -1) {
      perror("fork() failed");
      exit(1);
    }

    if (pid2 != 0) {
      transform(pipe1, pipe2, pid2);
    } else {
      display(pipe1, pipe2);
    }
  }
}
