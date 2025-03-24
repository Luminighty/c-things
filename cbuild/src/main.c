#include "rect.h"
#include "math.h"

int main() {
	Rect rect = rect_create(5, 10);
	int a = 5;
	int b = 15;
	printf("Math: %d + %d = %d\n", a, b, add(a, b));

	rect_print(&rect);
	printf("Area: %d\n", rect_area(&rect));

	return 0;
}
