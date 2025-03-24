#include <stdio.h>
#include <stdlib.h>

#define DATypedef(Type, Item) \
	typedef struct { \
		Item *items; \
		size_t count; \
		size_t capacity; \
	} Type


#define da_push(vec, value)\
	do {\
	if (vec.count >= vec.capacity) {\
		if (vec.capacity == 0) vec.capacity = 256;\
		else vec.capacity *= 2;\
		vec.items = realloc(vec.items, vec.capacity * sizeof(*vec.items));\
	}\
	vec.items[vec.count++] = value;\
	} while (0)

#define da_pop(vec) vec.items[--vec.count]

DATypedef(Numbers, int);

int main(void)
{
	Numbers ns = {0};
	for (int i = 0; i < 10; i++)
		da_push(ns, i);
	for (size_t i = 0; i < 10; i++)
		printf("%d\n", da_pop(ns));
}
