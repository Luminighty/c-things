#ifndef ECS_H
#define ECS_H

#include "component_registry.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define ENTITY_COUNT 120

typedef struct {
#define DENSE(type, name, _) type name [ENTITY_COUNT];
#define SPARSE(type, name, _, limit) type name [limit]; uint16_t name##_entity[limit]; uint16_t name##_count;
#define FLAG(type, name, _)
	COMPONENTS
#undef DENSE
#undef SPARSE
#undef FLAG
} ComponentContainer;

#define COMPONENTBITMAP_SLOTSIZE 64
typedef struct {
	uint64_t bytes[(COMPONENT_SIZE / COMPONENTBITMAP_SLOTSIZE) + 1];
} ComponentBitmap;

typedef union entity {
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


typedef struct world {
	ComponentContainer components;
	EntityContainer entities;
} World;


World world_create();
void world_print(World* world);
Entity entity_create(World* world);
void entity_destroy(World* world, Entity entity);
bool entity_is_alive(World* world, Entity entity);

bool entity_has_component(World *world, Entity entity, ComponentType component);
bool entity_has_components(World* world, Entity entity, ComponentBitmap components);

typedef struct {
	ComponentBitmap bitmap;
	Entity current;
} EntityQuery;

#define componentbitmap_create(...) _componentbitmap_create(__VA_ARGS__, COMPONENT_SIZE)
ComponentBitmap _componentbitmap_create(ComponentType c, ...);

// NOTE: I may need to generate this, but it's unused atm
#define is_query_empty(query) !(query .bytes[0])
#define StaticComponentBitmap(query) static ComponentBitmap query = {0}; if (is_query_empty(query)) query

Entity query_begin(World* world, ComponentBitmap query);
void query_next(World* world, Entity* entity, ComponentBitmap query);
bool query_not_end(World* world, Entity* entity);

#define foreach_ecs_query(world, entity, query) for (Entity entity = query_begin(world, query); query_not_end(world, &entity); query_next(world, &entity, query))

#endif // ECS_H
