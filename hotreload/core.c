#include "core.h"
#include <stdio.h>

void core_init(Game* game) {
	InitWindow(800, 600, "Reload");
	SetTargetFPS(60);
  game->x = 0;
  game->y = 0;
}

void core_update(Game* game) {
  int speed = 1;
  int dx = 0;
  int dy = 0;
  if (IsKeyDown(KEY_LEFT))
    dx--;
  if (IsKeyDown(KEY_RIGHT))
    dx++;
  if (IsKeyDown(KEY_UP))
    dy--;
  if (IsKeyDown(KEY_DOWN))
    dy++;
  game->x += dx * speed;
  game->y += dy * speed;
  BeginDrawing();
  ClearBackground(BLACK);
  DrawRectangle(game->x, game->y, 50, 50, RED);
  EndDrawing();
}