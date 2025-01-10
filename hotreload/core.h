#ifndef CORE_H_
#define CORE_H_

#include <raylib.h>

typedef struct {
	int x;
	int y;
} Game;

typedef void (*core_init_t)(Game* game);
typedef void (*core_update_t)(Game* game);

#endif // CORE_H_