#ifndef CPROC_PARSER
#define CPROC_PARSER

#include <cstddef>
#include <stddef.h>


typedef struct {
	bool is_pointer;
	char* type;
	char* field;
} MemberDefinition;


typedef struct {
	size_t count;
	size_t capacity;
	MemberDefinition* items;
} MemberDefinitions;


typedef struct {
	bool is_typedef;
	char* name;
	MemberDefinitions members;
} StructDefinition;


typedef struct {
	size_t count;
	size_t capacity;
	StructDefinition* items;
} StructDefinitions;

typedef void (*StructHandlerFn)(StructDefinition*);

typedef struct {
	char* tag;
	StructHandlerFn handler;
} StructHandler;


typedef struct {
	StructHandler* struct_handlers;
	size_t struct_handlers_count;
} ParserConfig;


void parse_file(ParserConfig* config, char* content);

#endif // CPROC_PARSER
