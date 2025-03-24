#include <fileapi.h>
#include <minwinbase.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
	char* args;
	int args_length;
	int args_capacity;
} CBuilder;

typedef struct {
	int build_argc;
	char** build_argv;
	char* mode;
} BuildEnv;

typedef struct {
	char* filename;
	bool end;
} SourceFile;

CBuilder builder = {0};
BuildEnv env = {0};

bool should_rebuild();
void rebuild();

void cmd(char* cmd);

void arg(char* argstr) {
	int len = strlen(argstr) + 1;
	if (len + builder.args_length >= builder.args_capacity) {
		int new_capacity = builder.args_capacity * 2;
		if (new_capacity == 0)
			new_capacity = 256;
		builder.args = realloc(builder.args, new_capacity);
		builder.args_capacity = new_capacity;
	}
	memcpy(builder.args + builder.args_length, argstr, len);
	builder.args_length += len;
	builder.args[builder.args_length - 1] = ' ';
	builder.args[builder.args_length] = '\0';
}


bool mode(char* argstr) {
	return strcmp(env.mode, argstr) == 0;
}


void build_init(int argc, char* argv[]) {
	env.build_argc = argc;
	env.build_argv = argv;
	if (argc == 1) {
		env.mode = "debug";
	} else {
		env.mode = argv[1];
	}
	if (should_rebuild())
		rebuild();
}

void next_file(SourceFile* file);
void first_file(SourceFile* file);

void build() {
	SourceFile current_file = {0};

	first_file(&current_file);
	while (!current_file.end) {
		printf("gcc %s %s\n", builder.args, current_file.filename);
	}

	printf("gcc %s\n", builder.args);
}

#ifdef _WIN32
#include <windef.h>
#include <WinBase.h>
#include <process.h>

#define get_attrs(file, attrs) \
	if (GetFileAttributesEx(file, GetFileExInfoStandard, &attrs) == 0) {\
		fprintf(stderr, "Failed to get "file" attributes: %lu\n", GetLastError());\
		return true;\
	}

bool should_rebuild() {
	WIN32_FILE_ATTRIBUTE_DATA buildlib_data;
	WIN32_FILE_ATTRIBUTE_DATA build_data;
	WIN32_FILE_ATTRIBUTE_DATA build_exec_data;
	get_attrs("build.exe", build_exec_data);
	get_attrs("build.c", build_data);
	get_attrs("buildlib.c", buildlib_data);
	if (CompareFileTime(&build_exec_data.ftLastWriteTime, &build_data.ftLastWriteTime) == -1)
		return true;
	if (CompareFileTime(&build_exec_data.ftLastWriteTime, &buildlib_data.ftLastWriteTime) == -1)
		return true;
	return false;
}

#undef get_attrs


void rebuild() {
	_execlp("cmd.exe", "cmd", "/C", "echo. && echo Rebuilding buildscript... && gcc build.c -o build && build", NULL);
	perror("Buildscript rebuild failed");
}

void cmd(char* cmd) {
	printf("$%s\n", cmd);
	_spawnlp(_P_NOWAIT, "cmd.exe", "cmd", "/C", cmd, NULL);
}

#else
bool should_rebuild() {
	printf("WARN: Unsupported build platform. Will always rebuild myself.\n");
	return true;
}
#endif
