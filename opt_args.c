#include <stdio.h>

typedef enum {
	UP    = 1 << 0,
	DOWN  = 1 << 1,
	LEFT  = 1 << 2,
	RIGHT = 1 << 3,
} Dir;

typedef enum {
	NO_LOOT,
	RUPEE,
	KEY,
	BOSS_KEY,
	MAP,
	BOMBS,
} ChestLoot;

typedef enum {
	SLIME,
	SKELETON,
	FIREWISP,
} EnemyKind;


// OPTIONAL ARGS HERE
typedef struct {
	int doors;
	int statue_count;
	EnemyKind enemy_kind;
	int enemy_count;
	ChestLoot loot;
} RoomCreateParams;
#define room_create(x, y, ...) \
	room_create_opt(x, y, ((RoomCreateParams){\
		.enemy_kind = SKELETON,\
		.enemy_count = 2,\
		__VA_ARGS__\
	}))
void room_create_opt(int x, int y, RoomCreateParams params);


int main() {
	room_create(2, 3);
	room_create(2, 4, .doors = UP | LEFT);
	room_create(3, 4, .statue_count = 2);
	room_create(5, 4, .enemy_kind = SLIME, .enemy_count = 12, .loot = MAP);
	return 0;
}


void room_create_opt(int x, int y, RoomCreateParams params) {
	printf("Creating room @(%d, %d):\n", x, y);
	
	printf("\tdoors: ");
	if (params.doors & UP) printf("U");
	if (params.doors & DOWN) printf("D");
	if (params.doors & LEFT) printf("L");
	if (params.doors & RIGHT) printf("R");
	printf("\n");

	printf("\tstatue_count: %d\n", params.statue_count);
	printf("\tEnemy: ");
	switch (params.enemy_kind) {
	case SLIME: printf("slime"); break;
	case SKELETON: printf("skeleton"); break;
	case FIREWISP: printf("fire_wisp"); break;
	}
	printf(" x %d\n", params.enemy_count);
	printf("\tLoot: ");

	switch (params.loot) {
	case NO_LOOT: printf("nil"); break;
	case RUPEE: printf("RUPEE"); break;
	case KEY: printf("KEY"); break;
	case BOSS_KEY: printf("BOSS_KEY"); break;
	case MAP: printf("MAP"); break;
	case BOMBS: printf("BOMBS"); break;
	}
	printf("\n");
}
