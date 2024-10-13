#ifndef ENUM_H
#define ENUM_H

typedef enum {
	ACTION_MESSAGE,
	ACTION_MOVE,
	ACTION_ATTACK,
} ActionType;

typedef struct {
	char message[32];
} ActionMessage;

typedef struct {
	int x;
	int y;
} ActionMove;

typedef struct {
	int enemy;
	short mode;
} ActionAttack;


typedef struct {
	ActionType action;
	union {
		ActionMessage message;
		ActionMove move;
		ActionAttack attack;
	};
} Action;

Action action_message(const char message[32]);
Action action_move(int x, int y);
Action action_attack(int enemy, short mode);

#endif