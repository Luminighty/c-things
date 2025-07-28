#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct {
	size_t size;
	size_t capacity;
	char* data;
} Arena;

typedef size_t ArenaPtr;


ArenaPtr arena_push(Arena* arena, size_t size) {
	if (arena->size + size >= arena->capacity) {
		if (arena->capacity == 0) arena->capacity = 64;
		else arena->capacity *= 2;
		printf("mallocing %zu\n", arena->capacity);
		arena->data = (char*)realloc(arena->data, sizeof(char) * arena->capacity);
	}
	ArenaPtr ptr = arena->size;
	arena->size += size;
	return ptr;
}


void arena_destroy(Arena* arena) {
	printf("destroying Used: %zu Reserved: %zu\n", arena->size, arena->capacity);
	if (arena->data != 0)
		free(arena->data);
}

void* arena_get(Arena* arena, ArenaPtr arena_ptr) {
	return arena->data + arena_ptr;
}


#endif // ARENA_H
