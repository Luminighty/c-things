#include <stdio.h>
#include "buildlib.c"

void clean();

int main(int argc, char* argv[]) {
	build_init(argc, argv);

	printf("Building project 3\n");

	if (mode("clean")) {
		clean();
		return 0;
	}

	arg("-I./include");
	arg("-Llibs -lraylib -lpthread -lm");

	if (mode("release"))
		arg("-g -O0 -Wall -Wextra");
	
	if (mode("debug"))
		arg("-O2 -DNDEBUG");

	cmd("mkdir bin 2>nul");
	cmd("cp -r assets bin 2>nul");
	build();
	printf("Done!\n");
}


void clean() {
	cmd("rm -rf build bin");
}
