#ifndef COMPONENTS_H
#define COMPONENTS_H
// THIS FILE IS AUTOGENERATED.

typedef struct { int x; int y; } Position;
typedef struct { int x; int y; int w; int h; } Rect;
typedef struct { char name[32]; } Player;

typedef enum {
  C_POSITION,
  C_RECT,
  C_PLAYER,
} ComponentType;

typedef struct {
  ComponentType type;
  union {
    Position position;
    Rect rect;
    Player player;
  } as;
} Component;

#endif // COMPONENTS_H