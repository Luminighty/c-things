#ifndef COLLISION_H
#define COLLISION_H

#include <raylib.h>
#include <stdint.h>


typedef uint32_t ColliderId;

typedef struct {
	Vector2 center;
	Vector2 extends;
} CollisionBox;


typedef struct {
	ColliderId id;
	CollisionBox box;
} Collider;


typedef struct {
	Vector2 origin;
	Vector2 delta;
} Ray2;


typedef struct {
	Vector2 origin;
	Vector2 delta;
	Vector2 inv;
} Ray2Extended;


typedef struct {
	bool collided;
	bool is_inside_box;
	ColliderId other;
	float distance;
	Vector2 resolved_position;
} ColliderMoveResult;

ColliderId collider_create(Vector2 center, Vector2 extends);
Collider* collider_get(ColliderId id);
ColliderMoveResult collider_move(ColliderId id, Vector2 delta);
Ray2Extended ray2_create_ex(Vector2 origin, Vector2 delta);
Ray2 ray2_create(Vector2 origin, Vector2 delta);

#endif // COLLISION_H
