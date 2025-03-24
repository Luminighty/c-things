#include "core.h"
#include <raylib.h>

#if HOT_RELOAD_WIN
#include "hot_reload_win.c"
#else 

#define HOT_RELOAD_INIT() // void
#define HOT_RELOAD_UPDATE(game, dt) // void
#define HOT_RELOAD_DESTROY() // void

void core_init(Game* game);
void core_update(Game* game);
void core_reload(Game* game);

#endif

Game game = {0};
int main() {
	HOT_RELOAD_INIT()

	core_init(&game);
	while (!WindowShouldClose()) {
		HOT_RELOAD_UPDATE(&game, GetTime())
		core_update(&game);
	}
	HOT_RELOAD_DESTROY()
	return 0;
}
