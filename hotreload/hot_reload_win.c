
#include <stdio.h>
#include <libloaderapi.h>
#include <windef.h>
#include <WinBase.h>
#include "core.h"

const char* dll_file_name = "core.dll";
const char* dll_temp_file_name = "core.dll.tmp";

HINSTANCE dll_handle = NULL;
core_init_t core_init;
core_update_t core_update;

void* get_fn_addrs(HINSTANCE dll_handle, const char* fn) {
	void* handle = (void *)GetProcAddress(dll_handle, fn);
	if (handle == NULL) {
		printf("Can't call %s dll function\n", fn);
		exit(1);
	}
	return handle;
}

HINSTANCE load_dll() {
	if (dll_handle != NULL) {
		FreeLibrary(dll_handle);
	}
	CopyFileA(dll_file_name, dll_temp_file_name, false);
	HINSTANCE dll_handle = LoadLibraryExA(dll_temp_file_name, NULL, 0);
	if (dll_handle == NULL) {
		printf("Can't load the dll file.\n");
		exit(1);
	}
	return dll_handle;
}

void reload_dll() {
	dll_handle = load_dll();
	core_init = get_fn_addrs(dll_handle, "core_init");
	core_update = get_fn_addrs(dll_handle, "core_update");
}

bool has_newer_dll(FILETIME* last_modified_time) {
	WIN32_FILE_ATTRIBUTE_DATA file_data;
	if (GetFileAttributesEx(dll_file_name, GetFileExInfoStandard, &file_data) == 0) {
		fprintf(stderr, "Failed to get file attributes: %lu", GetLastError());
		return false;
	}

	if (CompareFileTime(&file_data.ftLastWriteTime, last_modified_time) != 0) {
		*last_modified_time = file_data.ftLastWriteTime;
		return true;
	}
	return false;
}

static int dll_timer = 0;
static FILETIME last_modified_time = {0};

void hot_reload_init() {
	has_newer_dll(&last_modified_time);
	reload_dll();
}

void hot_reload_update(float time) {
	int new_dll_timer = (int)time;
	if (new_dll_timer != dll_timer) {
		if (has_newer_dll(&last_modified_time))
			reload_dll();\
		dll_timer = new_dll_timer;
	}
}

#define HOT_RELOAD_INIT() hot_reload_init();
#define HOT_RELOAD_UPDATE(dt) hot_reload_update(dt);
#define HOT_RELOAD_DESTROY() FreeLibrary(dll_handle);
