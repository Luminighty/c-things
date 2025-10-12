#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include "collision.h"
#include "utils.h"


bool is_drawing_box = false;
Collider* player;
ColliderId player_id;

ColliderId other_ids[10];
int other_c = 0;

void add_other(float x, float y, float w, float h) {
	ColliderId box_id = collider_create((Vector2){x, y}, (Vector2){w, h});
	other_ids[other_c++] = box_id;
}

int main() {
	player_id = collider_create((Vector2){100.f, 100.f}, (Vector2){25.f, 25.f});
	player = collider_get(player_id);

	add_other(360, 360, 100, 50);
	add_other(50, 50, 30, 30);
	add_other(500, 500, 50, 50);
	add_other(200, 300, 100, 25);

	add_other(20, 500, 40, 80);
	add_other(300, 300, 50, 50);
	add_other(400, 400, 50, 50);

	InitWindow(720, 720, "Collision");
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		Vector2 mouse_position = GetMousePosition();
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
			player->box.center = mouse_position;

		Vector2 target_position = GetMousePosition();
		ColliderMoveResult result = collider_move(player_id, Vector2Subtract(target_position, player->box.center));

		BeginDrawing();
		ClearBackground(BLACK);


		DrawLineV(player->box.center, target_position, result.collided ? RED : GREEN);
		if (result.collided) {
			draw_point(result.resolved_position.x, result.resolved_position.y, YELLOW);
			CollisionBox box = player->box;
			box.center = result.resolved_position;
			draw_box(box, YELLOW);
		}

		CollisionBox target = player->box;
		target.center = result.resolved_position;
		draw_box(target, result.collided ? YELLOW : GREEN);
		draw_box(player->box, GREEN);
		for (int other_id = 0; other_id < other_c; other_id++) {
			ColliderId id = other_ids[other_id];
			draw_box(collider_get(id)->box, result.other == id ? RED : WHITE);
		}

		EndDrawing();
		fflush(stdout);
	}
	return 0;
}
