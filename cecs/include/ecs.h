#ifndef ECS_H
#define ECS_H

#include "component_registry.h"
#include "components.h"
#include <stdint.h>
#include <stdbool.h>

#define ENTITY_COUNT 24

typedef struct {
#define X(type, name, _) type name [ENTITY_COUNT];
	COMPONENTS
#undef X
} ComponentContainer;

typedef uint32_t ComponentBitmap;

typedef union {
	uint32_t id;
	struct {
		uint16_t gen;
		uint16_t index;
	} as;
} Entity;


typedef struct {
	uint32_t entity_size;
	bool alive[ENTITY_COUNT];
	uint16_t generation[ENTITY_COUNT];
	ComponentBitmap component_map[ENTITY_COUNT];
} EntityContainer;


typedef struct {
	ComponentContainer components;
	EntityContainer entities;
} World;


World world_create();
void world_print(World* world);
Entity entity_create(World* world);
void entity_destroy(World* world, Entity entity);

bool entity_has_components(World* world, Entity entity, ComponentBitmap components);

Position* entity_add_position(World* world, Entity entity, Position position);
Renderable* entity_add_renderable(World* world, Entity entity, Renderable renderable);
Stats* entity_add_stats(World* world, Entity entity, Stats stats);
void entity_add_player(World* world, Entity entity);

Position* entity_get_position(World* world, Entity entity);
Renderable* entity_get_renderable(World* world, Entity entity);
Stats* entity_get_stats(World* world, Entity entity);

void entity_remove_position(World* world, Entity entity);
void entity_remove_renderable(World* world, Entity entity);
void entity_remove_stats(World* world, Entity entity);
void entity_remove_player(World* world, Entity entity);

typedef struct {
	ComponentBitmap bitmap;
	Entity current;
} EntityQuery;

Entity query_begin(World* world, ComponentBitmap query);
void query_next(World* world, Entity* entity, ComponentBitmap query);
bool query_not_end(World* world, Entity* entity);

#define foreach_ecs_query(world, entity, query) for (Entity entity = query_begin(world, query); query_not_end(world, &entity); query_next(world, &entity, query))

#endif // ECS_H
