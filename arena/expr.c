#include <stdio.h>

#include "arena.c"

typedef enum {
	BINOP_ADD,
	BINOP_SUB,
	BINOP_MUL,
	BINOP_DIV,
} BinOp;

typedef enum {
	EXPR_VAL,
	EXPR_BINARY,
} ExprKind;


typedef struct {
	BinOp op;
	ArenaPtr lhs;
	ArenaPtr rhs;
} BinaryExpr;


typedef struct {
	ExprKind kind;
	union {
		BinaryExpr binary;
		int value;
	} as;
} Expr;


ArenaPtr expr_val(Arena* arena, int value) {
	ArenaPtr ptr = arena_push(arena, sizeof(Expr));
	Expr* expr = arena_get(arena, ptr);
	expr->kind = EXPR_VAL;
	expr->as.value = value;
	return ptr;
}


ArenaPtr expr_binary(Arena* arena, BinOp op, ArenaPtr lhs, ArenaPtr rhs) {
	ArenaPtr ptr = arena_push(arena, sizeof(Expr));
	Expr* expr = arena_get(arena, ptr);
	expr->kind = EXPR_BINARY;
	expr->as.binary.lhs = lhs;
	expr->as.binary.rhs = rhs;
	expr->as.binary.op = op;
	return ptr;
}

static inline int binop_eval(BinOp op, int lhs, int rhs) {
	switch (op) {
	case BINOP_ADD:
		return lhs + rhs;
	case BINOP_SUB:
		return lhs - rhs;
	case BINOP_MUL:
		return lhs * rhs;
	case BINOP_DIV:
		return lhs / rhs;
	}
}

static char BINOP_CHAR[] = {
	[BINOP_ADD] = '+',
	[BINOP_MUL] = '*',
	[BINOP_DIV] = '/',
	[BINOP_SUB] = '-'
};

void expr_print(Arena* arena, ArenaPtr expr_ptr) {
	Expr* expr = arena_get(arena, expr_ptr);
	switch (expr->kind) {
	case EXPR_VAL:
		printf("%d", expr->as.value);
		return;
	case EXPR_BINARY:
		printf(" (");
		expr_print(arena, expr->as.binary.lhs);
		printf(" %c ", BINOP_CHAR[expr->as.binary.op]);
		expr_print(arena, expr->as.binary.rhs);
		printf(") ");
		return;
	}
}

int expr_eval(Arena* arena, ArenaPtr expr_ptr) {
	Expr* expr = arena_get(arena, expr_ptr);
	switch (expr->kind) {
	case EXPR_VAL:
		return expr->as.value;
	case EXPR_BINARY: {
		int lhs = expr_eval(arena, expr->as.binary.lhs);
		int rhs = expr_eval(arena, expr->as.binary.rhs);
		return binop_eval(expr->as.binary.op, lhs, rhs);
	}
	default:
		return 0;
	}
}

