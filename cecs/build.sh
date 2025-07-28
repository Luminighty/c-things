gcc ./scripts/

INCLUDE=./include/

gcc -I$(INCLUDE) ./src/ecs.c -c -o ./build/ecs.o
gcc -I$(INCLUDE) ./src/main.c -c -o ./build/main.o

gcc ./build/ecs.o ./build/main.o -o ./main

