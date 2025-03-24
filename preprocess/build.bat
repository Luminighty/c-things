gcc .\c_preprocessor.c -o .\cprep.exe
.\cprep.exe
rem gcc .\meta.c -c -o .\build\meta.o
gcc .\main.c -c -o .\build\main.o
gcc .\build\main.o -o main.exe