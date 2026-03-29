#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include "collision.h"
#include "utils.h"


bool is_drawing_box = false;
Collider* player;
ColliderId player_id;

ColliderId other_ids[20];
int other_c = 0;

static void add_other(float x, float y, float w, float h) {
	ColliderId box_id = collider_create((Vector2){x, y}, (Vector2){w, h});
	other_ids[other_c++] = box_id;
}

static void add_randoms() {
	add_other(360, 360, 100, 50);
	add_other(50, 50, 30, 30);
	add_other(500, 500, 50, 50);
	add_other(200, 300, 100, 25);

	add_other(20, 500, 40, 80);
	add_other(300, 300, 50, 50);
	add_other(400, 400, 50, 50);
}

static void add_tilemap() {
	static const float SIZE = 16;
	for (int i = 0; i < 10; i++) {
		add_other((SIZE * 5) + i * SIZE * 2, 400, SIZE, SIZE);
		add_other(100, (SIZE * 5) + i * SIZE * 2, SIZE, SIZE);
	}
}

static void test_case() {
	add_other(232, 280, 8, 8);
	add_other(248, 296, 8, 8);
}

static void raymarch_debug(Vector2 from, Vector2 to, Vector2 extends) {
	static const float SIZE = 50;
	// Vector2 delta = Vector2Subtract(to, from);

	Vector2 min = (Vector2){ 
		.x = floor((fmin(from.x, to.x) - extends.x) / SIZE) * SIZE, 
		.y = floor((fmin(from.y, to.y) - extends.y) / SIZE) * SIZE,
	};
	Vector2 max = (Vector2){ 
		.x = ((fmax(from.x, to.x) + extends.x) / SIZE) * SIZE,
		.y = ((fmax(from.y, to.y) + extends.y) / SIZE) * SIZE,
	};
	for (float y = min.y; y < max.y; y += SIZE)
	for (float x = min.x; x < max.x; x += SIZE)
		DrawRectangleLines(x, y, SIZE, SIZE, PURPLE);
}

int main() {
	player_id = collider_create((Vector2){244.f, 284.f}, (Vector2){4.f, 4.f});
	player = collider_get(player_id);

	// add_tilemap();
	test_case();

	InitWindow(720, 720, "Collision");
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		Vector2 mouse_position = GetMousePosition();
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
			player->box.center = mouse_position;

		// Vector2 delta = {1.5f, 1.5f};
		Vector2 delta = {3.f, 3.f};
		ColliderMoveResult result = collider_move(player_id, delta);
		Vector2 target_position = Vector2Add(player->box.center, delta);
		// Vector2 target_position = GetMousePosition();
		// ColliderMoveResult result = collider_move(player_id, Vector2Subtract(target_position, player->box.center));

		BeginDrawing();
		ClearBackground(BLACK);


		DrawLineV(player->box.center, target_position, result.collided ? RED : GREEN);
		if (result.collided) {
			draw_point(result.resolved_position.x, result.resolved_position.y, YELLOW);
			CollisionBox box = player->box;
			box.center = result.resolved_position;
			draw_box(box, YELLOW);
		}
		if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
			player->box.center = result.resolved_position;

		CollisionBox target = player->box;
		target.center = result.resolved_position;
		draw_box(target, result.collided ? YELLOW : GREEN);
		draw_box(player->box, GREEN);
		for (int other_id = 0; other_id < other_c; other_id++) {
			ColliderId id = other_ids[other_id];
			draw_box(collider_get(id)->box, result.other == id ? RED : WHITE);
		}
		// raymarch_debug(player->box.center, target_position, player->box.extends);

		EndDrawing();
		fflush(stdout);
	}
	return 0;
}
