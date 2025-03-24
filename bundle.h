#ifndef BUNDLE_L_H
#define BUNDLE_L_H

#include <stdint.h>

static const uint16_t VERSION = 0;

typedef struct {
	uint16_t version;
	size_t count;
	Resource* resources;
} Bundle;

typedef struct {
	uint32_t type;
	uint32_t id;
	void* data;
} Resource;

#endif // BUNDLE_L_H