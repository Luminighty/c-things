#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include "collision.h"
#include "utils.h"


bool is_drawing_box = false;
Collider* player;
ColliderId player_id;

int main() {
	player_id = collider_create((Vector2){100.f, 100.f}, (Vector2){25.f, 25.f});
	player = collider_get(player_id);

	ColliderId box_id = collider_create((Vector2){360.f, 360.f}, (Vector2){100.f, 50.f});
	Collider* box = collider_get(box_id);

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

		draw_box(player->box, GREEN);
		draw_box(box->box, WHITE);

		EndDrawing();
		fflush(stdout);
	}
	return 0;
}
