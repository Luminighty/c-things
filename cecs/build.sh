gcc ./scripts/component_parser.c -o ./scripts/component_parser
./scripts/component_parser

mkdir build

gcc -I./include/ ./src/ecs.c -c -o ./build/ecs.o
gcc -I./include/ ./src/component_registry.c -c -o ./build/component_registry.o
gcc -I./include/ ./src/main.c -c -o ./build/main.o

gcc ./build/ecs.o ./build/main.o ./build/component_registry.o -o ./main

