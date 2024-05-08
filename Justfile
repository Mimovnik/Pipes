
all:
  mkdir -p build
  gcc -o build/main main.c -Wall -fsanitize=leak,address

run:
  ./build/main

clean:
  rm build/main

