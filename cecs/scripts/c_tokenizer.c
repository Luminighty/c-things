#ifndef C_TOKENIZER_C_IMPLEMENTATION
#define C_TOKENIZER_C_IMPLEMENTATION

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


typedef enum {
	TOKEN_UNKNOWN,
	TOKEN_IDENTIFIER,
	TOKEN_OPEN_PAREN,
	TOKEN_CLOSE_PAREN,
	TOKEN_OPEN_BRACE,
	TOKEN_CLOSE_BRACE,
	TOKEN_OPEN_BRACKET,
	TOKEN_CLOSE_BRACKET,
	TOKEN_EQUAL,
	TOKEN_STAR,
	TOKEN_COLON,
	TOKEN_SEMICOLON,
	TOKEN_STRING,
	TOKEN_NUMBER,
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


bool is_whitespace(Tokenizer* tokenizer) {
	switch (tokenizer->at[0]) {
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				return true;
			default:
				return false;
	}
}


void consume_whitespace(Tokenizer* tokenizer) {
	while(1) {
		if (is_whitespace(tokenizer)) {
			tokenizer->at++;			
		} else if (tokenizer->at[0] == '/' && tokenizer->at[1] == '/') {
			tokenizer->at += 2;
			while (tokenizer->at[0] && tokenizer->at[0] != '\n' && tokenizer->at[0] != '\r')
				tokenizer->at++;
		} else if (tokenizer->at[0] == '/' && tokenizer->at[1] == '*') {
			tokenizer->at += 2;
			while (tokenizer->at[0] && !(tokenizer->at[0] == '*' && tokenizer->at[1] == '/'))
				tokenizer->at++;
			tokenizer->at++; // Skip over last /
		} else {
			return;
		}
	}
}


bool is_alpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}


bool is_numeric(char c) {
	// TODO(Lumi): Support for 0x, f, ., ect.
	return c >= '0' && c <= '9';
}


bool token_equals(Token token, char* match) {
	char* at = match;
	for (int i = 0; i < token.length; i++, at++) {
		if (*at == '\0' || *at != token.content[i])
			return false;
	}
	return *at == 0;
}


Token token_get(Tokenizer* tokenizer) {
	consume_whitespace(tokenizer);
	Token token;
	token.length = 1;
	token.content = tokenizer->at;
	char c = tokenizer->at[0];
	tokenizer->at++;
	switch(c) {
		case '\0': token.type = TOKEN_EOF; break;
		case '(': token.type = TOKEN_OPEN_PAREN; break;
		case ')': token.type = TOKEN_CLOSE_PAREN; break;
		case '[': token.type = TOKEN_OPEN_BRACKET; break;
		case ']': token.type = TOKEN_CLOSE_BRACKET; break;
		case '{': token.type = TOKEN_OPEN_BRACE; break;
		case '}': token.type = TOKEN_CLOSE_BRACE; break;
		case '*': token.type = TOKEN_STAR; break;
		case ':': token.type = TOKEN_COLON; break;
		case ';': token.type = TOKEN_SEMICOLON; break;
		case '=': token.type = TOKEN_EQUAL; break;
		case '"': {
			token.content = tokenizer->at;
			token.type = TOKEN_STRING;
			while (tokenizer->at[0] && tokenizer->at[0] != '"') {
				if (tokenizer->at[0] == '\\' && tokenizer->at[1])
					tokenizer->at++;
				tokenizer->at++;
			}
			token.length = tokenizer->at - token.content; // length
			if (tokenizer->at[0] == '"')
				tokenizer->at++;
		} break;
		default: {
			if (is_alpha(c)) {
				while (is_alpha(tokenizer->at[0]) || is_numeric(tokenizer->at[0]) || tokenizer->at[0] == '_')
					tokenizer->at++;
				token.length = tokenizer->at - token.content;
				token.type = TOKEN_IDENTIFIER;
			} else if (is_numeric(c)) {
				while (is_numeric(tokenizer->at[0]))
					tokenizer->at++;
				token.type = TOKEN_NUMBER;
				token.length = tokenizer->at - token.content;
			} else {
				token.type = TOKEN_UNKNOWN;
			}
		} break;
	}
	return token;
}


long int token_number(Token* token) {
	if (token->type != TOKEN_NUMBER)
		return 0;
	char* end;
	return strtol(token->content, &end, 10);
}


bool token_consume(Tokenizer* tokenizer, TokenType type) {
	Token token = token_get(tokenizer);
	bool result = token.type == type;
	if (!result)
		printf("ERROR: Got %.*s.\n", token.length, token.content);
	return result;
}


#endif // C_TOKENIZER_C_IMPLEMENTATION
