#include "./arena.c"
#include "./expr.c"
#include <stdio.h>


int main() {

	Arena arena = {0};

	#define BIN(OP, LHS, RHS) expr_binary(&arena, OP, LHS, RHS)
	#define VAL(val) expr_val(&arena, val)

	ArenaPtr expr = BIN(
		BINOP_MUL,
		BIN(
			BINOP_ADD,
			VAL(5),
			VAL(10)
		),
		BIN(
			BINOP_SUB,
			VAL(20),
			BIN(
				BINOP_DIV,
				VAL(100),
				VAL(20)
			)
		)
	);

	expr_print(&arena, expr);
	printf("\n");
	int result = expr_eval(&arena, expr);
	printf("Eval: %d\n", result);

	arena_destroy(&arena);
	return 0;
}

