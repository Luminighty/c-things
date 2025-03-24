#ifndef RECT_H
#define RECT_H

typedef struct {
	int w, h;
} Rect;

Rect rect_create(int w, int h);
void rect_print(Rect* rect);

#endif // RECT_H
