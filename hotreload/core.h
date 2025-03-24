#ifndef CORE_H_
#define CORE_H_

#include <raylib.h>

typedef struct {
  int sprite_c;
  int user_data;
  char error[32];
} Sprites;


typedef struct {
	int x;
	int y;
	Sprites sprites;
} Game;

typedef void (*core_init_t)(Game* game);
typedef void (*core_update_t)(Game* game);
typedef void (*core_reload_t)(Game* game);

#endif // CORE_H_