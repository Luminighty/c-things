#include "cproc.h"
#include "clex.h"
#include "stdlib.h"

char* string_create(char* source, int length) {
	char* str = malloc(length + 1);
	for(int i = 0; i < length; i++)
		str[i] = source[i];
	str[length] = '\0';
	return str;
}


void parse_derive_params(Tokenizer* tokenizer) {
	while(1) {
		Token token = token_get(tokenizer);
		if (token.type == TOKEN_EOF || token.type == TOKEN_CLOSE_PAREN)
			return;
		// TODO(Lumi): Actually parse the params
	}
}

void parse_member(Tokenizer* tokenizer, Token member_type_token) {
	bool parsing = true;
	bool is_pointer = false;
	char* ty_ident = string_create(member_type_token.content, member_type_token.length);
	while(parsing) {
		Token token = token_get(tokenizer);
		switch (token.type) {
			case TOKEN_STAR: {
				is_pointer = true;
			} break;
			case TOKEN_IDENTIFIER: {
				MemberDefinition* member = malloc(sizeof(MemberDefinition));
				member->is_pointer = is_pointer;
				member->type = ty_ident;
				member->field = string_create(token.content, token.length);
				member->next = current_struct->members;
				current_struct->members = member;
			} break;
			case TOKEN_EOF: {
				fprintf(stderr, "ERROR: Unexpected EOF.\n");
				return;
			} break;
			case TOKEN_SEMICOLON: {
				parsing = false;
			} break;
		}
	}
}

void parse_struct_body(Tokenizer* tokenizer) {
	if (!token_consume(tokenizer, TOKEN_OPEN_BRACE)) {
		fprintf(stderr, "ERROR: '{' expected.\n");
		return;
	}
	while(1) {
		Token token = token_get(tokenizer);
		if (token.type == TOKEN_EOF) {
			fprintf(stderr, "ERROR: '}' expected.\n");
			return;
		}
		if (token.type == TOKEN_CLOSE_BRACE)
			break;
		parse_member(tokenizer, token);
	}
	current_struct->members = reverse_members(current_struct->members);
}

void parse_struct(Tokenizer* tokenizer) {
	Token start = token_get(tokenizer);
	bool is_typedef = token_equals(start, "typedef");
	StructDefinition* new_struct = malloc(sizeof(StructDefinition));
	new_struct->next = current_struct;
	new_struct->is_typedef = is_typedef;
	new_struct->members = NULL;
	current_struct = new_struct;

	if (!is_typedef) {
		if (!token_equals(start, "struct")) {
			fprintf(stderr, "ERROR: 'struct' expected.\n");
			return;
		}
		Token name = token_get(tokenizer);
		new_struct->name = string_create(name.content, name.length);
		parse_struct_body(tokenizer);

	} else {
		if (!token_equals(token_get(tokenizer), "struct")) {
			fprintf(stderr, "ERROR: 'struct' expected.\n");
			return;
		}
		parse_struct_body(tokenizer);
		Token name = token_get(tokenizer);
		new_struct->name = string_create(name.content, name.length);
		try_push_type_ident(new_struct->name);
	}
}

void parse_derive(Tokenizer* tokenizer) {
	if (!token_consume(tokenizer, TOKEN_OPEN_PAREN)) {
		fprintf(stderr, "ERROR: Missing parentheses.\n");
		return;
	}
	parse_derive_params(tokenizer);
	parse_struct(tokenizer);
}


void parse_file(ParserConfig* config, char* content) {


}
