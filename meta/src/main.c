#include "components.h"
#include <stdio.h>

Component make_pos(int x, int y) {
	return ((Component){C_POSITION, { .position = {x, y}}});
	
}


Component make_rect(int x, int y, int w, int h) {
	return ((Component){C_RECT, { .rect = ((Rect){ x, y, w, h })}});
}

void print_component(Component* component) {

	switch (component->type) {
		case C_POSITION: 
			printf("POS(%d, %d)\n", component->as.position.x, component->as.position.y);
			break;
		case C_RECT: 
			printf("RECT(%d, %d, %d, %d)\n", component->as.rect.x, component->as.rect.y, component->as.rect.w, component->as.rect.h);
		break;
		case C_PLAYER:
			printf("PLAYER(%s)\n", component->as.player.name);
		break;
	}
}

int main() {
	Component components[] = {
		make_pos(10, 20),
		make_pos(30, 15),
	};
	int len = sizeof(components) / sizeof(Component);
	for (int i = 0; i < len; i++)
		print_component(&components[i]);
	return 0;
}