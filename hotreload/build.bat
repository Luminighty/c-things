set LDFLAGS=-lgdi32 -lwinmm -Llibs -lraylib -lpthread -lm -L./bin

mkdir bin

gcc -c -o core.o core.c
gcc -o ./bin/core.dll -shared core.c %LDFLAGS%