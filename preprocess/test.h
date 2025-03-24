#ifndef TEST_H_
#define TEST_H_

#include "preproc.h"


typedef struct {
	int x, y; 
} Vector2;

DERIVE(debug)
typedef struct {
	char asset;
	int r, g, b;
} Sprite;

DERIVE(debug)
typedef struct {
	Vector2 position;
	Vector2 size;
} Rect;

DERIVE(debug)
typedef struct {
	int id;
	Rect rect;
} Collision;

DERIVE(debug)
struct Player {
	Sprite sprite;
	Vector2 position;
	Collision collision;
	char name;
};


struct Player player_create();
Vector2 vec2_create(int x, int y);

#endif // TEST_H_
