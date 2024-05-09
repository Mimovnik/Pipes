
all:
  mkdir -p build
  gcc -o build/display display.c -Wall -fsanitize=leak,address
  gcc -o build/main main.c -Wall -fsanitize=leak,address

run:
  ./build/main

clean:
  rm build/main
  rm build/display

