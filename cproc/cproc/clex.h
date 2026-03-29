#ifndef CPROC_LEXER
#define CPROC_LEXER

#include <stdbool.h>

typedef enum {
	TOKEN_UNKNOWN,
	TOKEN_IDENTIFIER,
	TOKEN_OPEN_PAREN,
	TOKEN_CLOSE_PAREN,
	TOKEN_OPEN_BRACE,
	TOKEN_CLOSE_BRACE,
	TOKEN_OPEN_BRACKET,
	TOKEN_CLOSE_BRACKET,
	TOKEN_STAR,
	TOKEN_COLON,
	TOKEN_SEMICOLON,
	TOKEN_STRING,
	TOKEN_EOF,
} TokenType;

typedef struct {
	TokenType type;
	int length;
	char* content;
} Token;

typedef struct {
	char* at;
} Tokenizer;

bool is_whitespace(Tokenizer* tokenizer);
void consume_whitespace(Tokenizer* tokenizer);
bool is_alpha(char c);
bool is_numeric(char c);
bool token_equals(Token token, char* match);
Token token_get(Tokenizer* tokenizer);
bool token_consume(Tokenizer* tokenizer, TokenType type);


#endif // CPROC_LEXER
