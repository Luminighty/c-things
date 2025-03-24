typedef struct {
	const char* name;
	const char* fields[128];
} ComponentMeta;

ComponentMeta components[] = {
	{ "Position", {
		"int x", 
		"int y",
		0
	}},
	{ "Rect", {
		"int x", "int y",
		"int w", "int h",
		
		0
	}},
	{ "Player", {
		"char name[32]", 
		0
	}},
	{ "Ali", {
		"char abc[32]",
		0
	}},
};
