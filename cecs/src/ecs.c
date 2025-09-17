#include "ecs.h"
#include "component_registry.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

World world_create() {
	World w = {0};
	w.entities.entity_size = 1;
	return w;
}

Entity entity_create(World* world) {
	// TODO: Search for empty spots
	uint16_t idx = world->entities.entity_size++;
	world->entities.alive[idx] = true;
	uint16_t gen = ++world->entities.generation[idx];
	return (Entity){.as = {.gen = gen, .index = idx}};
}

bool entity_is_alive(World* world, Entity entity) {
	return world->entities.alive[entity.as.index] &&
		world->entities.generation[entity.as.index] == entity.as.gen;
}

void entity_destroy(World* world, Entity entity) {
	world->entities.alive[entity.as.index] = false;
}

static inline bool is_query_matches(ComponentBitmap entity_components, ComponentBitmap query) {
	for (int i = 0; i < (COMPONENT_SIZE / COMPONENTBITMAP_SLOTSIZE) + 1; i++)
		if (entity_components.bytes[i] != (entity_components.bytes[i] | query.bytes[i]))
			return false;
	return true;
}

bool entity_has_component(World *world, Entity entity, ComponentType component) {
	if (!entity_is_alive(world, entity))
		return false;
	ComponentBitmap entity_components = world->entities.component_map[entity.as.index];
	return entity_components.bytes[component / COMPONENTBITMAP_SLOTSIZE] & (component % COMPONENTBITMAP_SLOTSIZE);
}

bool entity_has_components(World* world, Entity entity, ComponentBitmap components) {
	if (!entity_is_alive(world, entity))
		return false;
	ComponentBitmap entity_components = world->entities.component_map[entity.as.index];
	return is_query_matches(entity_components, components);
}

ComponentBitmap _componentbitmap_create(ComponentType c, ...) {
    ComponentBitmap bitmap = {0};
    va_list args;
    for (va_start(args, c); c != COMPONENT_SIZE; c = va_arg(args, ComponentType)) {
        bitmap.bytes[c / COMPONENTBITMAP_SLOTSIZE] |= 1UL << (c % COMPONENTBITMAP_SLOTSIZE);
    }
    return bitmap;
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
		if (!is_query_matches(entity_components, query))
			continue;
		entity->as.index = index;
		entity->as.gen = world->entities.generation[index];
		return;
	}
}


bool query_not_end(World* world, Entity* entity) {
	return entity->as.index < world->entities.entity_size;
}

