#include "rect.h"
#include <stdio.h>


Rect rect_create(int w, int h) {
	return (Rect){
		.w = w,
		.h = h
	}
}


void rect_print(Rect* rect) {
	printf("Rect(%d, %d)\n", rect->w, rect->h);
}
