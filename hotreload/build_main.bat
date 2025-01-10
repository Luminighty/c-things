set LDFLAGS=-lgdi32 -lwinmm -Llibs -lraylib -lpthread -lm -L./bin
set DEFINES=-DHOT_RELOAD_WIN

mkdir bin

gcc -o ./bin/debug.exe main.c %LDFLAGS% %DEFINES%