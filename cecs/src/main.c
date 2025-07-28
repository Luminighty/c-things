#include <stdio.h>

#include "components.h"
#include "ecs.h"
#include "component_registry.h"


Entity player_create(World* world, int x, int y, bool add_stats) {
	Entity player = entity_create(world);
	entity_add_position(world, player, (Position){.x = x, .y = y});
	if (add_stats)
		entity_add_stats(world, player, (Stats){.hp = 100, .stamina = 100});
	entity_add_renderable(world, player, (Renderable){.glyph = '@', .fg = 1, .bg = 1});
	entity_add_player(world, player);
	return player;
}


Entity enemy_create(World* world) {
	Entity enemy = entity_create(world);
	entity_add_position(world, enemy, (Position){.x = 15, .y = 22});
	entity_add_stats(world, enemy, (Stats){.hp = 15, .stamina = 5});
	entity_add_renderable(world, enemy, (Renderable){.glyph = 'g', .fg = 2, .bg = 1});
	return enemy;
}


void system_render(World* world) {
	ComponentBitmap query = COMPONENT_POSITION | COMPONENT_RENDERABLE;
	foreach_ecs_query(world, entity, query) {
		Position* position = entity_get_position(world, entity);
		Renderable* renderable = entity_get_renderable(world, entity);
		printf("%c at (%d, %d)\n", renderable->glyph, position->x, position->y);
	}
}

void system_player(World* world) {
	ComponentBitmap query = COMPONENT_POSITION | COMPONENT_PLAYER;
	foreach_ecs_query(world, player, query) {
		Position* position = entity_get_position(world, player);
		position->x += 1;
		printf("Moved player: %d %d\n", position->x, position->y);
		if (entity_has_components(world, player, COMPONENT_STATS)) {
			Stats* stats = entity_get_stats(world, player);
			printf("\tHP: %d  STAMINA: %d\n", stats->hp, stats->stamina);
		}
	}
}

int main() {
	World world = world_create();

	Entity player = player_create(&world, 5, 10, true);
	Entity player_no_stats = player_create(&world, 2, 30, false);
	Entity enemy = enemy_create(&world);

	world_print(&world);
	printf("\nGAME START:\n");

	system_render(&world);
	system_player(&world);
	system_player(&world);
	system_player(&world);
	system_render(&world);

	return 0;
}

#include "ecs.c"
