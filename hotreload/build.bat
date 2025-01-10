set LDFLAGS=-lgdi32 -lwinmm -Llibs -lraylib -lpthread -lm -L./bin

mkdir bin

gcc -c -o ./build/core.o core.c
gcc -o ./bin/core.dll -shared ./build/core.o %LDFLAGS%