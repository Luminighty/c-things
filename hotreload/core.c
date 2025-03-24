#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Game* _game;
Sprites* sprites;

void core_reload(Game* game) {
  _game = game;
  sprites = &game->sprites;
}

void core_init(Game* game) {
	InitWindow(800, 600, "Reload");
	SetTargetFPS(60);
  game->x = 0;
  game->y = 0;
  game->sprites.sprite_c = 0;
  game->sprites.user_data = 69;
  strcpy(game->sprites.error, "What the dog doing");
  core_reload(game);
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

  if (IsKeyPressed(KEY_P)) {
    printf("Sprites(%d, %d, \"%s\")\n", sprites->sprite_c, sprites->user_data, sprites->error);
  }
  if (IsKeyPressed(KEY_O)) {
    const char* err = "Hello zipzap";
    printf("Setting error: %s\n", err);
    sprites->user_data++;
    strcpy(sprites->error, err);
  }
  
  game->x += dx * speed;
  game->y += dy * speed;
  BeginDrawing();
  ClearBackground(BLACK);
  DrawRectangle(game->x, game->y, 50, 50, GREEN);
  EndDrawing();
}