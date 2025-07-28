#ifndef COMPONENTS_H
#define COMPONENTS_H

#define Component(...)


Component()
typedef struct {
	int x;
	int y;
} Position;


Component()
typedef struct {
	char glyph;
	int fg;
	int bg;
} Renderable;


Component()
typedef struct {
	int hp;
	int stamina;
} Stats;


Component(storage=sparse, storage_size=8)
typedef struct {
	int turns;
	int damage;
} Poisoned;


Component(storage=tag, storage_size=8)
typedef struct {} Player;


#endif // COMPONENTS_H
