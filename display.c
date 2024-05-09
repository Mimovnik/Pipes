#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define INPUT_BUFFER_SIZE 64
#define EXIT 0
#define FIFO "mypipe"

int main() {
  int mypipe;
  mypipe = open(FIFO, O_RDONLY);
  while (1) {
    int status;
    printf("DISPLAY: Waiting for message.\n");
    read(mypipe, &status, sizeof(status));

    if (status == EXIT) {
      puts("DISPLAY: quits.\n");
      close(mypipe);
      exit(0);
    }
    char message[INPUT_BUFFER_SIZE];

    read(mypipe, &message, sizeof(message));
    printf("DISPLAY: Reads '%s' from named pipe.\n", message);
  }
}
