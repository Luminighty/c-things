set LDFLAGS=-Llibs -lraylib -lpthread -lm -lgdi32 -lwinmm
set CFLAGS=-O2 -DNDEBUG

mkdir build
mkdir bin

gcc -c -o ./build/core.o core.c %CFLAGS%
gcc -c -o ./build/main.o main.c %CFLAGS%
gcc -o ./bin/main.exe ./build/main.o ./build/core.o %LDFLAGS%