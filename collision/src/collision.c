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


static inline bool ray_box_intersect(CollisionBox box, Ray2 ray_inv, double* tmin, double* tmax) {
	Vector2 bmin = box_min(&box);
	Vector2 bmax = box_max(&box);

	double tx1 = (bmin.x - ray_inv.origin.x) * ray_inv.delta.x;
	double tx2 = (bmax.x - ray_inv.origin.x) * ray_inv.delta.x;

	*tmin = fmin(tx1, tx2);
	*tmax = fmax(tx1, tx2);

	double ty1 = (bmin.y - ray_inv.origin.y) * ray_inv.delta.y;
	double ty2 = (bmax.y - ray_inv.origin.y) * ray_inv.delta.y;

	*tmin = fmax(*tmin, fmin(ty1, ty2));
	*tmax = fmin(*tmax, fmax(ty1, ty2));

	return *tmax >= *tmin;
}

static Vector2 get_box_normal(CollisionBox* box, Ray2 ray_inv, double tmin) {
	Vector2 bmin = box_min(box);
	Vector2 bmax = box_max(box);

	double tx1 = (bmin.x - ray_inv.origin.x) * ray_inv.delta.x;
	double tx2 = (bmax.x - ray_inv.origin.x) * ray_inv.delta.x;
	double ty1 = (bmin.y - ray_inv.origin.y) * ray_inv.delta.y;
	double ty2 = (bmax.y - ray_inv.origin.y) * ray_inv.delta.y;

	if (tmin == tx1)
		return (Vector2){ -1, 0 };
	if (tmin == tx2)
		return (Vector2){ 1, 0 };
	if (tmin == ty1)
		return (Vector2){ 0, -1 };
	if (tmin == ty2)
		return (Vector2){ 0, 1 };
	return (Vector2){ 0, 0 };
}


ColliderMoveResult collider_move(ColliderId id, Vector2 delta) {
	if (Vector2LengthSqr(delta) < 0.0f)
		return (ColliderMoveResult){0};
	Ray2 ray = {
		.origin = colliders[id].box.center,
		.delta = delta,
	};
	Ray2 ray_inv = ray;
	ray_inv.delta = (Vector2){
		.x = 1.f / ray.delta.x,
		.y = 1.f / ray.delta.y,
	};
	ColliderMoveResult result = {0};
	result.distance = 1.0f;
	for (ColliderId i = 0; i < collider_count; i++) {
		if (id == i)
			continue;
		double tmin, tmax = 0;
		CollisionBox extended = colliders[i].box;
		extended.extends.x += colliders[id].box.extends.x;
		extended.extends.y += colliders[id].box.extends.y;
		if (!ray_box_intersect(extended, ray_inv, &tmin, &tmax))
			continue;
		double t_hit = (tmin < 0.0) ? tmax : tmin;
		if (t_hit < 0.0 || t_hit > result.distance)
			continue;
		result.distance = t_hit;
		result.collided = true;
		result.other = i;
	}
	if (result.collided) {
		result.resolved_position.x = ray.origin.x + ray.delta.x * result.distance;
		result.resolved_position.y = ray.origin.y + ray.delta.y * result.distance;
		// TODO: Update colliders[id].box.center
	}
	return result;
}


