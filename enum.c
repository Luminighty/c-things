#include <stdio.h>
#include <memory.h>
#include "enum.h"

void action_do(Action* action) {
	switch (action->action) {
	case ACTION_MESSAGE:
		printf("ACTION_MESSAGE: '%s'\n", action->message.message);
		break;
	case ACTION_ATTACK:
		printf("ACTION_ATTACK: target: %d, mode: %d\n", action->attack.enemy, action->attack.mode);
		break;
	case ACTION_MOVE:
		printf("ACTION_MOVE: x: %d, y: %d\n", action->move.x, action->move.y);
		break;
	}
}

void action_message_do(ActionMessage* message) {
		printf("ACTION_MESSAGE: '%s'\n", message->message);
}
void action_attack_do(ActionAttack* attack) {
		printf("ACTION_ATTACK: target: %d, mode: %d\n", attack->enemy, attack->mode);
}
void action_move_do(ActionMove* move) {
		printf("ACTION_MOVE: x: %d, y: %d\n", move->x, move->y);
}

void action_do_other(Action* action) {
	typedef void (*ActionFunc)(void*);
	ActionFunc funcs[] = {
		[ACTION_MESSAGE] = (ActionFunc)action_message_do,
		[ACTION_ATTACK] = (ActionFunc)action_attack_do,
		[ACTION_MOVE] = (ActionFunc)action_move_do,
	};
	funcs[action->action](&action->move);
}

int main() {
	Action message = action_message("gg wp");
	Action attack = action_attack(12, 4);
	Action move = action_move(22, 10);
	action_do(&message);
	action_do(&attack);
	action_do(&move);

	action_do_other(&attack);
	return 0;
}


Action action_message(const char message[32]) {
	Action action;
	action.action = ACTION_MESSAGE;
	memcpy(action.message.message, message, 32);
	return action;
}

Action action_move(int x, int y) {
	Action action;
	action.action = ACTION_MOVE;
	action.move.x = x;
	action.move.y = y;
	return action;
}

Action action_attack(int enemy, short mode) {
	Action action;
	action.action = ACTION_ATTACK;
	action.attack.enemy = enemy;
	action.attack.mode = mode;
	return action;
}
