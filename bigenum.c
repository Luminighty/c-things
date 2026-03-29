#include <stdint.h>
#include <stdio.h>

typedef struct {
	uint64_t bits[2];
} ComponentBitmap;

typedef enum {
	COMPONENT_PLAYER,
	COMPONENT_SPRITE,
	COMPONENT_RECT,
	COMPONENT_ENEMY,
	COMPONENT_POSITION = 65,
	COMPONENT_CAMERA = 66,
} ComponentFlag;

#define COMPONENT(slot, id) \
	(((id) >= (slot) * 64 && (id) < ((slot) + 1) * 64) ? (1ULL << ((id) % 64)) : 0ULL)

#define EXPAND(x) x
#define FOR_EACH_1(action, slot, x) action(slot, x)
#define FOR_EACH_2(action, slot, x, ...) action(slot, x) | EXPAND(FOR_EACH_1(action, slot, __VA_ARGS__))
#define FOR_EACH_3(action, slot, x, ...) action(slot, x) | EXPAND(FOR_EACH_2(action, slot, __VA_ARGS__))
#define FOR_EACH_4(action, slot, x, ...) action(slot, x) | EXPAND(FOR_EACH_3(action, slot, __VA_ARGS__))
#define FOR_EACH_5(action, slot, x, ...) action(slot, x) | EXPAND(FOR_EACH_4(action, slot, __VA_ARGS__))
// Add more if you expect more than 5 component args

#define GET_FOR_EACH_MACRO(_1,_2,_3,_4,_5,NAME,...) NAME
#define FOR_EACH(action, slot, ...) \
    EXPAND(GET_FOR_EACH_MACRO(__VA_ARGS__, FOR_EACH_5, FOR_EACH_4, FOR_EACH_3, FOR_EACH_2, FOR_EACH_1)(action, slot, __VA_ARGS__))

#define COMPONENT_FOR_SLOT(slot, ...) FOR_EACH(COMPONENT, slot, __VA_ARGS__)

#define COMPONENT_MAP(...) \
((ComponentBitmap){ \
    .bits = { \
        COMPONENT_FOR_SLOT(0, __VA_ARGS__), \
        COMPONENT_FOR_SLOT(1, __VA_ARGS__) \
    } \
})

int main() {
	uint64_t val = COMPONENT(1, 64);
	ComponentBitmap map = COMPONENT_MAP(
		COMPONENT_PLAYER,
		COMPONENT_POSITION,
		COMPONENT_SPRITE
	);

	printf("%x\n", val);
	printf("%x\n", map.bits[0]);
	printf("%x\n", map.bits[1]);
	return 0;
}
