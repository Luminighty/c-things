#include "meta.h"
#include "test.h"

#include <stdio.h>

void debug_struct(int member_count, MemberDefinition member_definitions[], void* struct_ptr, int indent) {
	for (int i = 0; i < member_count; i++) {
		MemberDefinition* member = member_definitions + i;
		void* member_ptr = (char*)struct_ptr + member->offset;
		for (int c = 0; c < indent; c++)
			printf("  ");
		
		switch(member->type) {
			case METATYPE_char: {
				printf("%s: %c\n", member->field_name, *(char*)member_ptr);
			} break;
			case METATYPE_int: {
				printf("%s: %d\n", member->field_name, *(int*)member_ptr);
			} break;
			case METATYPE_Vector2: {
				printf("%s: (%d, %d)\n", member->field_name, ((Vector2*)member_ptr)->x, ((Vector2*)member_ptr)->y);
			} break;

			#define X(metatype, sub_members) \
			case metatype: { \
				printf("%s: \n", member->field_name); \
				debug_struct(sizeof(sub_members) / sizeof(MemberDefinition), sub_members, member_ptr, indent+1); \
			} break;
				INCLUDE_METATYPE_CASES
			#undef CASE
			
			default:
				break;
		}
	}
}

int main() {
	struct Player player = {0};
	player.collision.id = 333;
	player.collision.rect.position.x = 20;
	player.collision.rect.position.y = 20;
	player.collision.rect.size.x = 120;
	player.collision.rect.size.y = 150;
	player.sprite.asset = 'x';
	player.sprite.r = 100;
	player.sprite.g = 22;
	player.sprite.b = 66;
	player.name = 'b';
	player.position.x = 12;
	player.position.y = 20;

	debug_struct(sizeof(members_Player) / sizeof(MemberDefinition), members_Player, &player, 0);
}