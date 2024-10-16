#include "json.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#ifdef JSON_DEBUG_ENABLED
	#define JSON_DEBUG(...) { printf(__VA_ARGS__); fflush(stdout); }
#else
	#define JSON_DEBUG(...) {}
#endif

#ifdef JSON_ERRORS_ENABLED
	#define JSON_ERROR(...) { fprintf(stderr, __VA_ARGS__); fflush(stdout); }
#else
	#define JSON_ERROR(...) {}
#endif


typedef enum {
	TOKEN_BRACE_OPEN,
	TOKEN_BRACE_CLOSE,
	TOKEN_BRACKET_OPEN,
	TOKEN_BRACKET_CLOSE,
	TOKEN_QUOTES,
	TOKEN_COMMA,
	TOKEN_COLON,
	TOKEN_UNKNOWN,
} JsonToken;

typedef struct {
	int position;
	int length;
	const char* data;
} JsonTokenizer;

static JsonValue* json_value(JsonTokenizer* tokenizer);
static JsonValue* string(JsonTokenizer* tokenizer);
static JsonString* string_value(JsonTokenizer* tokenizer);

static void whitespace(JsonTokenizer* tokenizer) {
	while(1) {
		switch(tokenizer->data[tokenizer->position]) {
		case ' ':
		case '\n':
		case '\r':
		case '\t':
			tokenizer->position++;
			break;
		default:
			return;
		}
	}
}

static char peek(JsonTokenizer* tokenizer, int offset) {
	if (tokenizer->position + offset >= tokenizer->length)
		return '\0';
	return tokenizer->data[tokenizer->position + offset];
}

static bool is_end(JsonTokenizer* tokenizer) {
	return tokenizer->position >= tokenizer->length;
}

static JsonToken peek_token(JsonTokenizer* tokenizer) {
	switch (tokenizer->data[tokenizer->position]) {
		case '{': return TOKEN_BRACE_OPEN;
		case '}': return TOKEN_BRACE_CLOSE;
		case '[': return TOKEN_BRACKET_OPEN;
		case ']': return TOKEN_BRACKET_CLOSE;
		case '"': return TOKEN_QUOTES;
		case ':': return TOKEN_COLON;
		case ',': return TOKEN_COMMA;
		default: return TOKEN_UNKNOWN;
	}
}

static bool consume(JsonTokenizer* tokenizer, JsonToken token) {
	if (peek_token(tokenizer) == token) {
		tokenizer->position++;
		return true;
	}
	return false;
}

static inline char current(JsonTokenizer* tokenizer) {
	return peek(tokenizer, 0);
}

bool is_numeric(char c) {
	return c == '-' || (c >= '0' &&  c <= '9');
}

static JsonValue* json_create() {
	JsonValue* json = malloc(sizeof(JsonValue));
	if (!json) {
		JSON_DEBUG("json: malloc failed while creating bool")
		exit(1);
	}
	memset(json, 0, sizeof(JsonValue));
	return json;
}

static JsonValue* bool_create(bool value) {
	JsonValue* json = json_create();
	json->json_type = JSON_BOOL;
	json->boolean = value;
	return json;
}

static JsonValue* null_create() {
	JsonValue* json = json_create();
	json->json_type = JSON_NULL;
	return json;
}

// True | False | Null
static JsonValue* literal(JsonTokenizer* tokenizer) {
	#define CMP(word, length) !memcmp(&tokenizer->data[tokenizer->position], word, length)
	if (CMP("true", 4)) {
		tokenizer->position += 4;
		return bool_create(true);
	}
	if (CMP("false", 5)) {
		tokenizer->position += 5;
		return bool_create(false);
	}
	if (CMP("null", 4)) {
		tokenizer->position += 4;
		return null_create();
	}
	return NULL;
}

static JsonValue* object(JsonTokenizer* tokenizer) {
	JsonValue* object = json_create();
	object->json_type = JSON_OBJECT;
	object->object.entry_count = 0;
	object->object.entry_capacity = 8;
	object->object.entries = malloc(sizeof(JsonKeyValue) * 8);

	tokenizer->position++; // step over the {
	whitespace(tokenizer);
	if (peek_token(tokenizer) == TOKEN_BRACE_CLOSE) {
		tokenizer->position++; // step over the }
		return object;
	}
	while(1) {
		JsonString* key = string_value(tokenizer);
		if (!key) {
			JSON_DEBUG("json: object key returned null\n");
			json_destroy(object);
			return NULL;
		}

		whitespace(tokenizer);
		if (!consume(tokenizer, TOKEN_COLON)) {
			JSON_DEBUG("json: object expected colon, got %c\n", current(tokenizer));
			json_destroy(object);
			free(key);
			return NULL;
		}
		whitespace(tokenizer);

		JsonValue* value = json_value(tokenizer);
		if (!value) {
			JSON_DEBUG("json: object value returned null\n");
			json_destroy(object);
			free(key);
			return NULL;
		}

		if (object->object.entry_count >= object->object.entry_capacity) {
			object->object.entry_capacity *= 2;
			object->object.entries = realloc(object->object.entries, sizeof(JsonKeyValue) * object->object.entry_capacity);
		}

		int c = object->object.entry_count;
		object->object.entries[c].key = key;
		object->object.entries[c].value = value;
		object->object.entry_count++;

		whitespace(tokenizer);

		if (peek_token(tokenizer) == TOKEN_BRACE_CLOSE)
			break;

		if (!consume(tokenizer, TOKEN_COMMA)) {
			JSON_ERROR("json: object expected comma, got %c\n", current(tokenizer));
			json_destroy(object);
			return NULL;
		}
		whitespace(tokenizer);
	 }
	tokenizer->position++; // step over the }
	return object;
}

static JsonValue* array(JsonTokenizer* tokenizer) {
	JsonValue* array = json_create();
	array->json_type = JSON_ARRAY;
	array->array.length = 0;
	array->array.capacity = 8;
	array->array.array = malloc(sizeof(JsonKeyValue) * 8);

	if (!consume(tokenizer, TOKEN_BRACKET_OPEN)) {
		JSON_DEBUG("json: array expected bracket open, got %c\n", current(tokenizer));
		return NULL;
	}
	whitespace(tokenizer);
	if (peek_token(tokenizer) == TOKEN_BRACKET_CLOSE) {
		tokenizer->position++; // step over the ]
		return array;
	}
	while(1) {
		JsonValue* element = json_value(tokenizer);
		if (!element) {
			JSON_DEBUG("json: array value returned null\n");
			json_destroy(array);
			return NULL;
		}
		if (array->array.length >= array->array.capacity) {
			array->array.capacity *= 2;
			array->array.array = realloc(array->array.array, sizeof(JsonValue*) * array->array.capacity);
		}

		array->array.array[array->array.length++] = element;
		whitespace(tokenizer);

		if (peek_token(tokenizer) == TOKEN_BRACKET_CLOSE)
			break;

		if (!consume(tokenizer, TOKEN_COMMA)) {
			JSON_DEBUG("json: array expected comma\n");
			json_destroy(array);
			return NULL;
		}
		whitespace(tokenizer);
	 }
	tokenizer->position++; // step over the ]
	return array;
}

static JsonString* string_value(JsonTokenizer* tokenizer) {
	if (!consume(tokenizer, TOKEN_QUOTES)) {
		JSON_DEBUG("json: string expected quotes, got %c", current(tokenizer));
		return NULL;
	}
	int start = tokenizer->position;
	int length = 0;
	int offset = 0;
	for (offset = 0; ; offset++) {
		char c = peek(tokenizer, offset);
		if (c == '\0') {
			JSON_DEBUG("json: string eof detected\n");
			return NULL;
		}
		if (c == '\\')
			offset++; // Skip "\"
		if (c == '"')
			break;
		length++;
	}
	JSON_DEBUG("json: string length %d\n", length);
	JsonString* string = malloc(sizeof(JsonString) + sizeof(char) * (length + 1));
	memset(string->string, '\0', length);
	string->length = 0;
	for (int i = 0; i < offset; i++) {
		char c = peek(tokenizer, i);
		if (c == '\\') {
			char escaped = peek(tokenizer, ++i);
			switch (escaped) {
			case 'n': c = '\n'; break;
			case 'b': c = '\b'; break;
			case 'f': c = '\f'; break;
			case 'r': c = '\r'; break;
			case 't': c = '\t'; break;
			case '\\': c = '\\'; break;
			case 'u': JSON_ERROR("json: u is not supported\n"); break;
			default: break;
			}
		}
		string->string[string->length++] = c;
	}
	string->string[string->length++] = '\0';
	JSON_DEBUG("json: string: '%s'\n", string->string);
	tokenizer->position = start + offset + 1;
	return string;
}

static JsonValue* string(JsonTokenizer* tokenizer) {
	JsonValue* value = json_create();
	value->json_type = JSON_STRING;
	value->string = string_value(tokenizer);
	return value;
}

static JsonValue* number(JsonTokenizer* tokenizer) {
	int length = 0;
	while(1) {
		char c = tokenizer->data[tokenizer->position + length];
		if (!is_numeric(c))
			break;
		length++;
	}

	char c = tokenizer->data[tokenizer->position + length];
	if (c == '.') {
		length++;
		while(1) {
			char c = tokenizer->data[tokenizer->position + length];
			if (!is_numeric(c))
				break;
			length++;
		}
	}
	JsonValue* json = json_create();
	json->json_type = JSON_NUMBER;
	json->number = strtod(&tokenizer->data[tokenizer->position], NULL);
	tokenizer->position += length;
	return json;
}

static JsonValue* json_value(JsonTokenizer* tokenizer) {
	fflush(stdout);
	if (is_numeric(current(tokenizer)))
		return number(tokenizer);

	{
		JsonValue* literal_value = literal(tokenizer);
		if (literal_value)
			return literal_value;
	}

	switch (peek_token(tokenizer)) {
		case TOKEN_BRACE_OPEN: return object(tokenizer);
		case TOKEN_BRACKET_OPEN: return array(tokenizer);
		case TOKEN_QUOTES: return string(tokenizer);
		default:
			JSON_DEBUG("json: invalid token found\n");
			return NULL;
	}
}


JsonValue* json_loadf(const char* filename) {
	FILE* file = fopen(filename, "rb");
	if (file == NULL) {
		JSON_ERROR("Could not open file \"%s\".\n", filename);
		exit(74);
	}
	fseek(file, 0L, SEEK_END);

	size_t file_size = ftell(file);
	rewind(file);

	char* buffer = (char*)malloc(file_size + 1);
	if (buffer == NULL) {
		JSON_ERROR("Not enough memory to read \"%s\".\n", filename);
		exit(74);
	}
	size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
	if (bytes_read < file_size) {
		JSON_ERROR("Could not read file \"%s\".\n", filename);
		exit(74);
	}
	buffer[bytes_read] = '\0';

	fclose(file);
	JsonValue* value = json_load(buffer);
	free(buffer);
	return value;
}

JsonValue* json_load(const char* data) {
	JSON_DEBUG("Loading %s\n", data);
	JsonTokenizer tokenizer;
	tokenizer.data = data;
	tokenizer.position = 0;
	tokenizer.length = strlen(data);
	return json_value(&tokenizer);
}

void json_destroy(JsonValue *json) {
	switch (json->json_type) {
	case JSON_OBJECT: {
		for (int i = 0; i < json->object.entry_count; i++) {
			free(json->object.entries[i].key);
			json_destroy(json->object.entries[i].value);
		}
		free(json->object.entries);
		break;
	}
	case JSON_ARRAY: {
		for (int i = 0; i < json->array.length; i++)
			json_destroy(json->array.array[i]);
		free(json->array.array);
		break;
	}
	case JSON_STRING:
		free(json->string);
		break;
	case JSON_NULL:
	case JSON_NUMBER:
	case JSON_BOOL:
		break;
	}
	free(json);
}

JsonValue* json_object_get(JsonValue* json, const char* member) {
	assert(JSON_IS_OBJECT(json));
	for (int i = 0; i < json->object.entry_count; i++)
		if (!strcmp(member, json->object.entries[i].key->string))
			return json->object.entries[i].value;
	return NULL;
}

JsonValue* json_array_get(JsonValue* json, int index) {
	assert(JSON_IS_ARRAY(json));
	if (json->array.length > index && index >= 0)
		return json->array.array[index];
	return NULL;
}


int json_array_length(JsonValue* json) {
	assert(JSON_IS_ARRAY(json));
	return json->array.length;
}

JsonString* json_as_string(JsonValue* json) {
	assert(JSON_IS_STRING(json));
	return json->string;
}

double json_as_number(JsonValue* json) {
	assert(JSON_IS_STRING(json));
	return json->number;
}

bool json_as_bool(JsonValue* json) {
	assert(JSON_IS_BOOL(json));
	return json->boolean;
}

void json_print(JsonValue *json) {
	switch (json->json_type) {
	case JSON_OBJECT: {
		printf("{");
		for (int i = 0; i < json->object.entry_count; i++) {
			printf("\"%s\": ", json->object.entries[i].key->string);
			json_print(json->object.entries[i].value);
			if (i < json->array.length - 1)
				printf(", ");
		}
		printf("}");
		break;
	}
	case JSON_ARRAY: {
		printf("[");
		for (int i = 0; i < json->array.length; i++) {
			json_print(json->array.array[i]);
			if (i < json->array.length - 1)
				printf(", ");
		}
		printf("]");
		break;
	}
	case JSON_STRING:
		printf("\"%s\"", json->string->string);
		break;
	case JSON_NULL:
		printf("null");
		break;
	case JSON_NUMBER:
		printf("%f", json->number);
		break;
	case JSON_BOOL:
		if (json->boolean) {
			printf("true");
		} else {
			printf("false");
		}
		break;
	}
}
