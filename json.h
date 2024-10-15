#ifndef JSON_L_H
#define JSON_L_H

#include <stdbool.h>

typedef enum {
	JSON_NULL,
	JSON_STRING,
	JSON_BOOL,
	JSON_NUMBER,
	JSON_OBJECT,
	JSON_ARRAY,
} JsonType;


typedef struct {
	int length;
	char string[];
} JsonString;


typedef struct {
	JsonString* key;
	struct JsonValue* value;
} JsonKeyValue;


typedef struct {
	int entry_count;
	int entry_capacity;
	JsonKeyValue* entries;
} JsonObject;


typedef struct {
	int length;
	int capacity;
	struct JsonValue** array;
} JsonArray;


typedef struct JsonValue {
	JsonType json_type;
	union {
		JsonString* string;
		double number;
		bool boolean;
		JsonObject object;
		JsonArray array;
	};
} JsonValue;


JsonValue* json_loadf(const char* filename);
JsonValue* json_load(const char* data);
JsonValue* json_object_get(JsonValue* json, const char* member);

int json_array_length(JsonValue* json);
JsonValue* json_array_get(JsonValue* json, int index);
JsonString* json_as_string(JsonValue* json);
double json_as_number(JsonValue* json);
bool json_as_bool(JsonValue* json);

void json_destroy(JsonValue* json);
void json_print(JsonValue* json);

#define JSON_IS_BOOL(json) (json->json_type == JSON_BOOL)
#define JSON_IS_NUMBER(json) (json->json_type == JSON_NUMBER)
#define JSON_IS_STRING(json) (json->json_type == JSON_STRING)
#define JSON_IS_NULL(json) (json->json_type == JSON_NULL)
#define JSON_IS_OBJECT(json) (json->json_type == JSON_OBJECT)
#define JSON_IS_ARRAY(json) (json->json_type == JSON_ARRAY)

#endif // JSON_L_H