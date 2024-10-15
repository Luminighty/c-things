#include "json.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	const char* data;
	int position;
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

static JsonToken peek(JsonTokenizer* tokenizer) {
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
	if (peek(tokenizer) == token) {
		tokenizer->position++;
		return true;
	}
	return false;
}

static inline char current(JsonTokenizer* tokenizer) {
	return tokenizer->data[tokenizer->position];
}

bool is_numeric(char c) {
	return c == '-' || (c >= '0' &&  c <= '9');
}

static JsonValue* json_create() {
	JsonValue* json = malloc(sizeof(JsonValue));
	if (!json) {
		printf("json: malloc failed while creating bool");
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
	if (peek(tokenizer) == TOKEN_BRACE_CLOSE) {
		tokenizer->position++; // step over the }
		return object;
	}
	while(1) {
		JsonString* key = string_value(tokenizer);
		if (!key) {
			printf("json: object key returned null\n");
			return NULL;
		}

		whitespace(tokenizer);
		if (!consume(tokenizer, TOKEN_COLON)) {
			printf("json: object expected colon, got %c\n", current(tokenizer));
			return NULL;
		}
		whitespace(tokenizer);

		JsonValue* value = json_value(tokenizer);
		if (!value) {
			printf("json: object value returned null\n");
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

		if (peek(tokenizer) == TOKEN_BRACE_CLOSE)
			break;

		if (!consume(tokenizer, TOKEN_COMMA)) {
			printf("json: object expected comma, got %c\n", current(tokenizer));
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
		printf("json: array expected bracket open, got %c\n", current(tokenizer));
		return NULL;
	}
	whitespace(tokenizer);
	if (peek(tokenizer) == TOKEN_BRACKET_CLOSE) {
		tokenizer->position++; // step over the ]
		return array;
	}
	while(1) {
		JsonValue* element = json_value(tokenizer);
		if (!element) {
			printf("json: array value returned null\n");
			return NULL;
		}
		if (array->array.length >= array->array.capacity) {
			array->array.capacity *= 2;
			array->array.array = realloc(array->array.array, sizeof(JsonValue*) * array->array.capacity);
		}

		array->array.array[array->array.length++] = element;
		whitespace(tokenizer);

		if (peek(tokenizer) == TOKEN_BRACKET_CLOSE)
			break;

		if (!consume(tokenizer, TOKEN_COMMA)) {
			printf("json: array expected comma\n");
			return NULL;
		}
		whitespace(tokenizer);
	 }
	tokenizer->position++; // step over the ]
	return array;
}

static JsonString* string_value(JsonTokenizer* tokenizer) {
	if (!consume(tokenizer, TOKEN_QUOTES)) {
		printf("json: string expected quotes, got %c", current(tokenizer));
		return NULL;
	}
	int start = tokenizer->position;
	while (1) {
		switch (current(tokenizer)) {
		case '"': {

			int length = tokenizer->position - start;
			JsonString* string = malloc(sizeof(JsonString) + sizeof(char) * (length + 1));
			if (!string) {
				printf("json: str alloc failed\n");
				return NULL;
			}
			memset(string->string, '\0', length);
			string->length = length + 1;
			for (int i = 0; i < length; i++) {
				char c = tokenizer->data[tokenizer->position - length + i];
				string->string[i] = c;
			}
			string->string[length] = '\0';
			tokenizer->position++;
			return string;
		}
		// case '\\'
		default:
			tokenizer->position++;
			break;
		}
	}
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
	if (is_numeric(current(tokenizer)))
		return number(tokenizer);

	{
		JsonValue* literal_value = literal(tokenizer);
		if (literal_value)
			return literal_value;
	}

	switch (peek(tokenizer)) {
		case TOKEN_BRACE_OPEN: return object(tokenizer);
		case TOKEN_BRACKET_OPEN: return array(tokenizer);
		case TOKEN_QUOTES: return string(tokenizer);
		default:
			printf("json: invalid token found\n");
			return NULL;
	}
}


JsonValue* json_loadf(const char* filename) {
	FILE* file = fopen(filename, "rb");
	if (file == NULL) {
		fprintf(stderr, "Could not open file \"%s\".\n", filename);
		exit(74);
	}
	fseek(file, 0L, SEEK_END);

	size_t file_size = ftell(file);
	rewind(file);

	char* buffer = (char*)malloc(file_size + 1);
	if (buffer == NULL) {
		fprintf(stderr, "Not enough memory to read \"%s\".\n", filename);
		exit(74);
	}
	size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
	if (bytes_read < file_size) {
		fprintf(stderr, "Could not read file \"%s\".\n", filename);
		exit(74);
	}
	buffer[bytes_read] = '\0';

	fclose(file);
	JsonValue* value = json_load(buffer);
	free(buffer);
	return value;
}

JsonValue* json_load(const char* data) {
	printf("Loading %s\n", data);
	JsonTokenizer tokenizer;
	tokenizer.data = data;
	tokenizer.position = 0;
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
