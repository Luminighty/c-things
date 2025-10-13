#include "collision.h"
#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

Collider colliders[32] = {0};
ColliderId collider_count = 0;

ColliderId collider_create(Vector2 center, Vector2 extends) {
	ColliderId id = collider_count++;
	colliders[id].id = id;
	colliders[id].box = (CollisionBox){.center = center, .extends = extends};
	return id;
}
Collider* collider_get(ColliderId id) { return &colliders[id]; }


static inline Vector2 box_min(CollisionBox* box) {
	return (Vector2){
		.x = box->center.x - box->extends.x,
		.y = box->center.y - box->extends.y,
	};
}


static inline Vector2 box_max(CollisionBox* box) {
	return (Vector2){
		.x = box->center.x + box->extends.x,
		.y = box->center.y + box->extends.y,
	};
}


static inline bool ray_box_intersect(CollisionBox box, Ray2Extended ray, double* t_min, double* t_max) {
	Vector2 bmin = box_min(&box);
	Vector2 bmax = box_max(&box);

	double tx1 = (bmin.x - ray.origin.x) * ray.inv.x;
	double tx2 = (bmax.x - ray.origin.x) * ray.inv.x;

	*t_min = fmin(tx1, tx2);
	*t_max = fmax(tx1, tx2);

	double ty1 = (bmin.y - ray.origin.y) * ray.inv.y;
	double ty2 = (bmax.y - ray.origin.y) * ray.inv.y;

	*t_min = fmax(*t_min, fmin(ty1, ty2));
	*t_max = fmin(*t_max, fmax(ty1, ty2));

	return *t_max >= *t_min;
}


static inline Vector2 ray_box_normal(CollisionBox box, Ray2Extended ray, double t_hit) {
	Vector2 bmin = box_min(&box);
	Vector2 bmax = box_max(&box);

	double t_x1 = (bmin.x - ray.origin.x) * ray.inv.x;
	if (t_x1 == t_hit) return (Vector2){.x = -1, .y = 0};

	double t_x2 = (bmax.x - ray.origin.x) * ray.inv.x;
	if (t_x2 == t_hit) return (Vector2){.x = 1, .y = 0};

	double t_y1 = (bmin.y - ray.origin.y) * ray.inv.y;
	if (t_y1 == t_hit) return (Vector2){.x = 0, .y = -1};

	double t_y2 = (bmax.y - ray.origin.y) * ray.inv.y;
	if (t_y2 == t_hit) return (Vector2){.x = 0, .y = 1};

	return (Vector2){.x = 0, .y = 0};
}


static inline ColliderMoveResult run_pass(ColliderId id, Ray2Extended ray) {
	ColliderMoveResult result = {
		.collided = false,
		.is_inside_box = false,
		.distance = 1.0f,
		.resolved_position = {
			.x = colliders[id].box.center.x + ray.delta.x,
			.y = colliders[id].box.center.y + ray.delta.y,
		}
	};
	for (ColliderId other_id = 0; other_id < collider_count; other_id++) {
		if (id == other_id)
			continue;
		double t_min, t_max = 0;
		CollisionBox extended = colliders[other_id].box;
		extended.extends.x += colliders[id].box.extends.x;
		extended.extends.y += colliders[id].box.extends.y;
		if (!ray_box_intersect(extended, ray, &t_min, &t_max))
			continue;
		double t_hit = (t_min < 0.0) ? t_max : t_min;
		if (t_hit < 0.0 || t_hit > result.distance)
			continue;
		result.distance = t_hit;
		result.collided = true;
		result.other = other_id;
		result.is_inside_box = t_min < 0.0f;
	}
	if (!result.collided)
		return result;
	float distance = result.distance + (result.is_inside_box ? EPSILON : -EPSILON);
	result.resolved_position.x = ray.origin.x + ray.delta.x * distance;
	result.resolved_position.y = ray.origin.y + ray.delta.y * distance;
	return result;
}


static inline Vector2 vector2_reject(Vector2 a, Vector2 b) {
	float scale = Vector2DotProduct(a, b) / Vector2DotProduct(b, b);
	Vector2 proj_a = Vector2Scale(b, scale);
	return Vector2Subtract(a, proj_a);
}


#define DEBUG_ONLY
#define ENABLE_SLIDE
ColliderMoveResult collider_move(ColliderId id, Vector2 delta) {
	if (Vector2LengthSqr(delta) < 0.0f)
		return (ColliderMoveResult){0};
	Ray2Extended ray = ray2_create_ex(colliders[id].box.center, delta);
	ColliderMoveResult result = run_pass(id, ray);
	if (!result.collided)
		return result;

	#ifdef DEBUG_ONLY
	// NOTE: When debugging, we'll revert the box to the original state
	CollisionBox temp_box = colliders[id].box;
	#endif
	
	#ifdef ENABLE_SLIDE
	ColliderId prev_id = result.other;
	CollisionBox extended = colliders[result.other].box;
	extended.extends.x += colliders[id].box.extends.x;
	extended.extends.y += colliders[id].box.extends.y;
	Vector2 normal = ray_box_normal(extended, ray, result.distance);
	delta = Vector2Scale(delta, 1.0f - result.distance);
	delta = vector2_reject(delta, normal);
	colliders[id].box.center = result.resolved_position;
	ray = ray2_create_ex(colliders[id].box.center, delta);
	result = run_pass(id, ray);

	if (!result.collided) {
		result.other = prev_id;
		result.collided = true;
		colliders[id].box.center = result.resolved_position;
	}
	#endif // ENABLE_SLIDE

	#ifdef DEBUG_ONLY
	colliders[id].box = temp_box;
	#endif
	return result;
}


inline Ray2Extended ray2_create_ex(Vector2 origin, Vector2 delta) {
	return (Ray2Extended){
		.origin = origin,
		.delta = delta,
		.inv = {
			.x = 1.f / delta.x,
			.y = 1.f / delta.y,
		}
	};
}


inline Ray2 ray2_create(Vector2 origin, Vector2 delta) {
	return (Ray2){
		.origin = origin,
		.delta = delta,
	};
}
