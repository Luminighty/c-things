#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define STR_LEN 64
#define FILE_INPUT "./include/components.toml"
#define COMPONENT_HEADER "component_registry.h"
#define COMPONENT_SOURCE "component_registry.c"


#define da_type(type) typedef struct { type *items; int capacity; int count; }
#define da_push(list, item)\
    if (list .count >= list.capacity) {\
        if (list .capacity == 0) list .capacity = 32;\
        else list .capacity *= 2;\
        list .items = realloc(list .items, sizeof(list .items[0]) * list .capacity);\
    }\
    list .items[list .count++] = item;

#define error(fmt, ...) fprintf(stderr, "[Error %d:%d]: " fmt "\n", line, col, __VA_ARGS__); exit_code = 1

// ////////////////////////////////
// ////// Component Typedefs //////
// ////////////////////////////////

typedef struct {
    char content[STR_LEN];
} Field;
da_type(Field) Fields;

typedef enum {
    STORAGE_DENSE,
    STORAGE_SPARSE,
    STORAGE_FLAG,
} StorageKind;

typedef struct {
    char class_name[STR_LEN];
    char class_name_upper[STR_LEN];
    char class_name_lower[STR_LEN];
    Fields fields;
    StorageKind storage;
    int storage_limit;
} Component;

da_type(Component) Components;

Components components = {0};
Component component = {0};


FILE *file;
FILE *source;
char current;
int exit_code = 0;
int line, col = 0;

// //////////////////////////
// ////// Parser Utils //////
// //////////////////////////

void next() {
    if (current == '\n') {
        line++;
        col = 0;
    }
    col++;
    current = fgetc(file);
}
bool has_next() { return current != EOF; }

void consume_whitespace() {
    while (1) {
    switch (current) {
        case ' ': case '\n': case '\r': case '\t':
            next();
            break;
        default:
            return;
    }
    }
}

void read_until(char c) {
    while(has_next() && current != c)
        next();
}

void consume_newline() {
    while (1) {
    switch (current) {
        case '\n':
        case '\r':
            next();
            break;
        default:
            return;
    }
    }
}

void consume_char(char c) {
    if (current != c) {
        error("Expected '%c', but got '%c'", c, current);
        read_until('\n');
    } else {
        next();
    }
}

void identifier(char* target) {
    int i = 0;
    while (has_next() && i < STR_LEN) {
        target[i++] = current;
        next();
        if (!isalnum(current) && current != '_')
            return;
    }
    if (i >= STR_LEN)
        error("Identifier reached maximum string length! (max_length: %d)", STR_LEN);
}

void number(int* number) {
    *number = 0;
    while (has_next() && current >= '0' && current <= '9') {
        *number *= 10;
        *number += current - '0';
        next();
    }
}

// //////////////////////////////
// ////// Component Parser //////
// //////////////////////////////

void parse_tag_storage() {
    consume_whitespace();
    consume_char('=');
    consume_whitespace();
    char tag_value[STR_LEN] = {0};
    identifier(tag_value);
    if (strcmp(tag_value, "sparse") == 0) {
        component.storage = STORAGE_SPARSE;
    } else if (strcmp(tag_value, "dense") == 0) {
        component.storage = STORAGE_DENSE;
    } else if (strcmp(tag_value, "flag") == 0) {
        component.storage = STORAGE_FLAG;
    } else {
        error("Undefined storage type: \"%s\"", tag_value);
        read_until('\n');
    }
}


void parse_tag() {
    consume_char('@');

    char tag_name[STR_LEN] = {0};
    identifier(tag_name);

    if (strcmp(tag_name, "storage") == 0) {
        parse_tag_storage();
    } else if (strcmp(tag_name, "limit") == 0) {
        consume_whitespace();
        consume_char('=');
        consume_whitespace();
        int limit = 0;
        number(&limit);
        component.storage_limit = limit;
    } else {
        error("Undefined tag \"@%s\"", tag_name);
        read_until('\n');
    }
}

void parse_field() {
    Field field = {0};
    int offset = 0;
    while (has_next() && current != '\n' && offset < STR_LEN) {
        field.content[offset++] = current;
        next();
    }
    if (offset >= STR_LEN) {
        error("Line reached maximum string length! (max_length: %d)", STR_LEN);
        return;
    }

    da_push(component.fields, field);
}

void parse_component() {
    memset(&component, 0, sizeof(component));

    consume_whitespace();
    consume_char('[');
    identifier(component.class_name);
    consume_char(']');
    consume_newline();
    consume_whitespace();

    while(has_next()) {
        if (current == '[') {
            break;
        } else if (current == '@') {
            parse_tag();
        } else {
            parse_field();
        }
        consume_whitespace();
    }
    int offset = 0;
    for (int i = 0; component.class_name[i] != '\0'; i++) {
        if (i > 0 && isupper(component.class_name[i])) {
            component.class_name_lower[offset] = '_';
            component.class_name_upper[offset] = '_';
            offset++;
        }
        component.class_name_lower[offset] = tolower(component.class_name[i]);
        component.class_name_upper[offset] = toupper(component.class_name[i]);
        offset++;
    }
    da_push(components, component);
}

// /////////////////////////
// ////// File output //////
// /////////////////////////


void print_component(Component* component) {
    switch(component->storage) {
        default:
        case STORAGE_DENSE:  fprintf(file, "// DENSE\n"); break;
        case STORAGE_FLAG:   fprintf(file, "// FLAG\n"); break;
        case STORAGE_SPARSE: fprintf(file, "// SPARSE\n"); break;
    }
    fprintf(file, "// LIMIT: %d\n", component->storage_limit);
    fprintf(file, "typedef struct {\n");
    for (int i = 0; i < component->fields.count; i++)
      fprintf(file, "\t%s;\n", component->fields.items[i].content);
    fprintf(file, "} %s;\n\n", component->class_name);
}

void print_component_type_enum() {
    fprintf(file, "typedef enum {\n");
    for (int i = 0; i < components.count; i++) {
        fprintf(file, "\tCOMPONENT_");
        for (int j = 0; components.items[i].class_name[j] != '\0'; j++)
            putc(toupper(components.items[i].class_name[j]), file);
        fprintf(file, ",\n");
    }
    fprintf(file, "\tCOMPONENT_SIZE\n");
    fprintf(file, "} ComponentType;\n\n");
}


void print_source_macros() {
    fprintf(source, "\
#define ENTITY_SET_TAG(world, entity, component)\\\n\
	world->entities.component_map[entity.as.index].bytes[component / COMPONENTBITMAP_SLOTSIZE] |= (component %% COMPONENTBITMAP_SLOTSIZE)\n\n");

    fprintf(source, "\
#define ENTITY_UNSET_TAG(world, entity, component)\\\n\
	world->entities.component_map[entity.as.index].bytes[component / COMPONENTBITMAP_SLOTSIZE] &= ~(component %% COMPONENTBITMAP_SLOTSIZE);\n\n");

    fprintf(source, "\
#define ENTITY_ADD_DENSE(world, entity, container, data, component_tag)\\\n\
	if (!entity_is_alive(world, entity))\\\n\
		return &world->components.container[0];\\\n\
	world->components.container[entity.as.index] = data;\\\n\
	ENTITY_SET_TAG(world, entity, component_tag);\\\n\
	return &world->components.container[entity.as.index]\n\n");

    fprintf(source, "\
#define ENTITY_ADD_SPARSE(world, entity, container, data, component_tag)\\\n\
	if (!entity_is_alive(world, entity))\\\n\
		return &world->components.container[0];\\\n\
        size_t index = world->components.container##_count++;\\\n\
	world->components.container##_entity[index] = entity.as.index;\\\n\
	world->components.container[index] = data;\\\n\
	ENTITY_SET_TAG(world, entity, component_tag);\\\n\
	return &world->components.container[index]\n\n");

    fprintf(source, "\
#define ENTITY_ADD_FLAG(world, entity, container, data, component_tag)\\\n\
	if (!entity_is_alive(world, entity))\\\n\
		return;\\\n\
	ENTITY_SET_TAG(world, entity, component_tag);\n\n");

    fprintf(source, "\
#define ENTITY_REMOVE(world, entity, component_tag)\\\n\
	if (!entity_is_alive(world, entity))\\\n\
		return;\\\n\
	ENTITY_UNSET_TAG(world, entity, component_tag);\n\n");

    fprintf(source, "\
#define ENTITY_REMOVE_SPARSE(world, entity, component_tag)\\\n\
	if (!entity_is_alive(world, entity))\\\n\
		return;\\\n\
	ENTITY_UNSET_TAG(world, entity, component_tag);\n\n");
}

void print_component_functions(Component* component) {
    // TODO: Add support for sparse arrays
    // TODO: Check for flags

    // entity_get_component
    if (component->storage != STORAGE_FLAG) {
        fprintf(file, "#define entity_get_%s(world, entity)\\\n", component->class_name_lower);
        fprintf(file, "  entity_is_alive(world, entity) ? &world->components.%s[entity.as.index] : &world->components.%s[0]\n", component->class_name_lower, component->class_name_lower);
    }

    // entity_add_component
    switch (component->storage) {
    default:
    case STORAGE_DENSE:
        fprintf(file, "%s* entity_add_%s(struct world* _world, union entity _entity, %s %s);\n", component->class_name, component->class_name_lower, component->class_name, component->class_name_lower);
        fprintf(source, "%s* entity_add_%s(struct world* _world, union entity _entity, %s %s) { ", component->class_name, component->class_name_lower, component->class_name, component->class_name_lower);
        fprintf(source, "ENTITY_ADD_DENSE(_world, _entity, %s, %s, COMPONENT_%s); }\n", component->class_name_lower, component->class_name_lower, component->class_name_upper);
        break;
    case STORAGE_SPARSE:
        fprintf(file, "%s* entity_add_%s(struct world* _world, union entity _entity, %s %s);\n", component->class_name, component->class_name_lower, component->class_name, component->class_name_lower);
        fprintf(source, "%s* entity_add_%s(struct world* _world, union entity _entity, %s %s) { ", component->class_name, component->class_name_lower, component->class_name, component->class_name_lower);
        fprintf(source, "ENTITY_ADD_SPARSE(_world, _entity, %s, %s, COMPONENT_%s); }\n", component->class_name_lower, component->class_name_lower, component->class_name_upper);
        break;
    case STORAGE_FLAG:
        fprintf(file, "void entity_add_%s(struct world* _world, union entity _entity);\n", component->class_name_lower);
        fprintf(source, "void entity_add_%s(struct world* _world, union entity _entity) { ", component->class_name_lower);
        fprintf(source, "ENTITY_ADD_FLAG(_world, _entity, %s, %s, COMPONENT_%s); }\n", component->class_name_lower, component->class_name_lower, component->class_name_upper);
        break;
    }

    // entity_remove_component
    fprintf(file, "void entity_remove_%s(struct world* _world, union entity _entity);\n", component->class_name_lower);
    fprintf(source, "void entity_remove_%s(struct world* _world, union entity _entity) { ", component->class_name_lower);
    fprintf(source, "ENTITY_REMOVE(_world, _entity, COMPONENT_%s) }\n", component->class_name_upper);

    fprintf(source, "\n");
    fprintf(file, "\n");
}

void print_component_xmacro() {
    fprintf(file, "#define COMPONENTS\\\n");
    for (int i = 0; i < components.count; i++) {
        fprintf(file, " ");
        switch (components.items[i].storage) {
        case STORAGE_DENSE: 
            fprintf(file, "DENSE(%s, %s, COMPONENT_%s)", components.items[i].class_name, components.items[i].class_name_lower, components.items[i].class_name_upper);
            break;
        case STORAGE_SPARSE: 
            fprintf(file, "SPARSE(%s, %s, COMPONENT_%s, %d)", components.items[i].class_name, components.items[i].class_name_lower, components.items[i].class_name_upper, components.items[i].storage_limit);
            break;
        case STORAGE_FLAG:
            fprintf(file, "FLAG(%s, %s, COMPONENT_%s)", components.items[i].class_name, components.items[i].class_name_lower, components.items[i].class_name_upper);
            break;
        }
        if (i + 1 < components.count)
            fprintf(file, "\\\n");
    }
    fprintf(file, "\n\n");
}


int main() {
    file = fopen(FILE_INPUT, "r");
    next();
    while(has_next())
        parse_component();
    fclose(file);

    file = fopen("./include/" COMPONENT_HEADER, "w");
    source = fopen("./src/" COMPONENT_SOURCE, "w");
    fprintf(source, "// This file has been autogenerated using component_parser.\n");
    fprintf(source, "#include \"" COMPONENT_HEADER "\"\n");
    fprintf(source, "#include \"ecs.h\"\n\n");
    print_source_macros();

    fprintf(file, "// This file has been autogenerated using component_parser.\n");
    fprintf(file, "#ifndef COMPONENT_REGISTRY_H\n");
    fprintf(file, "#define COMPONENT_REGISTRY_H\n\n");
    for (int i = 0; i < components.count; i++)
        print_component(&components.items[i]);
    print_component_type_enum();
    print_component_xmacro();

    fprintf(file, "union entity;\n");
    fprintf(file, "struct world;\n");
    fprintf(file, "\n");
    for (int i = 0; i < components.count; i++)
        print_component_functions(&components.items[i]);

    fprintf(file, "#endif // COMPONENT_REGISTRY_H\n");

    return exit_code;
}
