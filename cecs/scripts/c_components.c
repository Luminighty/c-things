#include "c_tokenizer.c"


typedef enum {
	STORAGE_DENSE,
	STORAGE_SPARSE,
	STORAGE_TAG,
} StorageType;


typedef struct {
	StorageType storage;
	int storage_size;
} ComponentParams;

ComponentParams component_params = {0};


void parse_storage_type(Tokenizer* tokenizer) {
	if (!token_consume(tokenizer, TOKEN_EQUAL))
		return;
	
	Token token = token_get(tokenizer);
	if (token_equals(token, "tag")) {
		component_params.storage = STORAGE_TAG;
	} else if (token_equals(token, "sparse")) {
		component_params.storage = STORAGE_SPARSE;
	} else if (token_equals(token, "dense")) {
		component_params.storage = STORAGE_DENSE;
	}
}

void parse_storage_size(Tokenizer* tokenizer) {
	if (!token_consume(tokenizer, TOKEN_EQUAL))
		return;
	Token token = token_get(tokenizer);
	component_params.storage_size = token_number(&token);
}

void parse_component_params(Tokenizer* tokenizer) {
	component_params.storage = STORAGE_DENSE;
	component_params.storage_size = 0;

	while(1) {
		Token token = token_get(tokenizer);
		if (token.type == TOKEN_EOF || token.type == TOKEN_CLOSE_PAREN)
			return;
		if (token_equals(token, "storage"))
			parse_storage_type(tokenizer);
		if (token_equals(token, "storage_size"))
			parse_storage_size(tokenizer);
	}
}

