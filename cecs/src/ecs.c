#include "ecs.h"
#include <stdint.h>
#include <stdio.h>

World world_create() {
	World w = {0};
	w.entities.entity_size = 1;
	return w;
}

void world_print(World* world) {
	printf("Entities\n");
	for(int i = 0; i < world->entities.entity_size; i++) {
		printf("Entity(%d gen%d):\n", i, world->entities.generation[i]);
		printf("    alive? %d\n", world->entities.alive[i]);
		printf("    components: ");
		#define X(type, _, TAG)  if (world->entities.component_map[i] & TAG) printf(#type ", ");
		COMPONENTS
		#undef X
		printf("\n");
	}

	#define print_component_type(Type, container, TAG, format, ...)\
	printf(#Type ":\n");\
	for (int i = 0; i < world->entities.entity_size; i++) {\
		if (!(world->entities.component_map[i] & TAG))\
			continue;\
		printf("    Entity (%d gen%d): ", i, world->entities.generation[i]);\
		Type c = world->components.container[i];\
		printf(format"\n", __VA_ARGS__);\
	}

	print_component_type(Position, positions, COMPONENT_POSITION, "%d, %d", c.x, c.y)
	print_component_type(Renderable, renderables, COMPONENT_RENDERABLE, "%c (fg: %d, bg: %d)", c.glyph, c.fg, c.bg);
	print_component_type(Stats, stats, COMPONENT_STATS, "HP: %d, STAMINA: %d", c.hp, c.stamina);
}

Entity entity_create(World* world) {
	// TODO: Search for empty spots
	uint16_t idx = world->entities.entity_size++;
	world->entities.alive[idx] = true;
	uint16_t gen = ++world->entities.generation[idx];
	return (Entity){.as = {.gen = gen, .index = idx}};
}

static inline bool is_entity_alive(World* world, Entity entity) {
	return world->entities.alive[entity.as.index] &&
		world->entities.generation[entity.as.index] == entity.as.gen;
}

void entity_destroy(World* world, Entity entity) {
	world->entities.alive[entity.as.index] = false;
}

bool entity_has_components(World* world, Entity entity, ComponentBitmap components) {
	if (!is_entity_alive(world, entity))
		return false;
	ComponentBitmap entity_components = world->entities.component_map[entity.as.index];
	return entity_components == (entity_components | components);
}

#define ENTITY_SET_TAG(world, entity, component) \
	world->entities.component_map[entity.as.index] |= component

#define ENTITY_ADD(world, entity, container, data, component_tag)\
	if (!is_entity_alive(world, entity))\
		return &world->components.container[0];\
	world->components.container[entity.as.index] = data;\
	ENTITY_SET_TAG(world, entity, component_tag);\
	return &world->components.container[entity.as.index]

Position* entity_add_position(World* world, Entity entity, Position data) {
	ENTITY_ADD(world, entity, positions, data, COMPONENT_POSITION);
}
Renderable* entity_add_renderable(World* world, Entity entity, Renderable data) {
	ENTITY_ADD(world, entity, renderables, data, COMPONENT_RENDERABLE);
}
Stats* entity_add_stats(World* world, Entity entity, Stats data) {
	ENTITY_ADD(world, entity, stats, data, COMPONENT_STATS);
}

void entity_add_player(World* world, Entity entity) {
	if (!is_entity_alive(world, entity))\
		return;
	ENTITY_SET_TAG(world, entity, COMPONENT_PLAYER);
}

#define ENTITY_GET(world, entity, container)\
	if (!is_entity_alive(world, entity))\
		return &world->components.container[0];\
	return &world->components.container[entity.as.index]

Position* entity_get_position(World* world, Entity entity) {
	ENTITY_GET(world, entity, positions);
}
Renderable* entity_get_renderable(World* world, Entity entity) {
	ENTITY_GET(world, entity, renderables);
}
Stats* entity_get_stats(World* world, Entity entity) {
	ENTITY_GET(world, entity, stats);
}

#define ENTITY_UNSET_TAG(world, entity, component) \
	world->entities.component_map[entity.as.index] &= ~component;
void entity_remove_position(World* world, Entity entity) {
	if (!is_entity_alive(world, entity))\
		return;
	ENTITY_UNSET_TAG(world, entity, COMPONENT_POSITION);
}


void entity_remove_renderable(World* world, Entity entity) {
	if (!is_entity_alive(world, entity))\
		return;
	ENTITY_UNSET_TAG(world, entity, COMPONENT_POSITION);
}


void entity_remove_stats(World* world, Entity entity) {
	if (!is_entity_alive(world, entity))\
		return;
	ENTITY_UNSET_TAG(world, entity, COMPONENT_POSITION);
}


void entity_remove_player(World* world, Entity entity) {
	if (!is_entity_alive(world, entity))\
		return;
	ENTITY_UNSET_TAG(world, entity, COMPONENT_POSITION);
}


Entity query_begin(World* world, ComponentBitmap query) {
	Entity entity = {0};
	query_next(world, &entity, query);
	return entity;
}


void query_next(World* world, Entity* entity, ComponentBitmap query) {
	uint16_t index = entity->as.index;
	while (1) {
		index++;
		if (index >= world->entities.entity_size) {
			entity->as.index = index;
			entity->as.gen = 0;
			return;
		}
		if (!world->entities.alive[index])
			continue;
		ComponentBitmap entity_components = world->entities.component_map[index];
		if (entity_components != (entity_components | query))
			continue;
		entity->as.index = index;
		entity->as.gen = world->entities.generation[index];
		return;
	}
}


bool query_not_end(World* world, Entity* entity) {
	return entity->as.index < world->entities.entity_size;
}

