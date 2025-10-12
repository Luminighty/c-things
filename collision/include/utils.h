#ifndef UTILS_H
#define  UTILS_H


#include "collision.h"
#include <raylib.h>


static const int POINT_SIZE = 5;
static inline void draw_point(int x, int y, Color color) {
	DrawLine(x - POINT_SIZE, y - POINT_SIZE, x + POINT_SIZE, y + POINT_SIZE, color);
	DrawLine(x + POINT_SIZE, y - POINT_SIZE, x - POINT_SIZE, y + POINT_SIZE, color);
}


static inline void draw_box(CollisionBox box, Color color) {
	int x = box.center.x - box.extends.x;
	int y = box.center.y - box.extends.y;
	int w = box.extends.x * 2;
	int h = box.extends.y * 2;
	DrawRectangleLines(x, y, w, h, color);
	draw_point(box.center.x, box.center.y, color);
}


#endif // UTILS_H
