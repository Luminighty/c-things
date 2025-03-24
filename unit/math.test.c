#define MODULE math
#define TEST(name) void math_ ## name()
#define str(s) #s
#define xstr(s) str(s)
//#define ON_ASSERT_FAILED return;
#define ON_ASSERT_FAILED
#define assert(value, message) if (!(value)) { \
	fprintf(stderr, __FILE__ ":" xstr(__LINE__) ": Assertion failed!\n"); \
	fprintf(stderr, "\t--> assert(" xstr(value) ")"); \
	ON_ASSERT_FAILED \
}

#define assert_eq(got, expected) if (got != expected) { \
	fprintf(stderr, __FILE__ ":" xstr(__LINE__) ": Assertion failed!\n"); \
	fprintf(stderr, "\t--> assert_eq(" xstr(got) ", " xstr(expected) "): %d != %d\n", got, expected); \
	ON_ASSERT_FAILED \
}

#include <stdio.h>

TEST(abc) {
	int result = 0;
	//assert(result)
	assert_eq(result, 10);
	assert_eq(result, 10);
}


typedef struct {
	int x, y, z;
} Vec;

#define ADD_VEC(vec, _x, _y, _z) \
 (vec).x += _x;\
 (vec).y += _y;\
 (vec).z += _z;

#define PRINT_VEC(vec) printf("(%d, %d, %d)\n", (vec).x, (vec).y, (vec).z)

void run(Vec* vec) {
	PRINT_VEC(*vec);
	ADD_VEC(*vec, 5, 5, 5);
	PRINT_VEC(*vec);
}

void main() {
	math_abc();

	Vec v = {.x=10, .y=20, .z=30};
	ADD_VEC(v, 5, 5, 5);
	PRINT_VEC(v);

	run(&v);
	
	PRINT_VEC(v);
}