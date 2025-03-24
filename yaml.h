#ifndef YAML_L_H
#define YAML_L_H

#include <stdbool.h>


// #define YAML_DEBUG_ENABLED
#define YAML_ERRORS_ENABLED

typedef enum {
	YAML_NULL,
	YAML_STRING,
	YAML_BOOL,
	YAML_NUMBER,
	YAML_MAP,
	YAML_LIST,
} YamlType;


typedef struct {
	int length;
	char string[];
} YamlString;


typedef struct {
	YamlString* key;
	struct YamlValue* value;
} YamlKeyValue;


typedef struct {
	int entry_count;
	int entry_capacity;
	YamlKeyValue* entries;
} YamlMap;


typedef struct {
	int length;
	int capacity;
	struct YamlValue** array;
} YamlList;


typedef struct YamlValue {
	YamlType yaml_type;
	union {
		YamlString* string;
		double number;
		bool boolean;
		YamlMap map;
		YamlList list;
	};
} YamlValue;


#endif // YAML_L_H