#include <json.h>
#include <errno.h>

char* get_section(char *title, char *json)
{
	char delim = '';
	for (int level = 0; level < 4; level++) {
	switch (level) {
		case 0:
			delim = ']';
			break;
		case 1:
			delim = '}';
			break;
		case 2:
			delim = ')';
			break;
		default:
			return -EINVAL;
	}
	int size = 1;
	char* token = strtok(json, delim);
	while ((token = strtok(NULL, delim)+2) != NULL) {
		if(!strncmp(token, title, strlen(title)))
			return token + strlen(title) + 2;
	}
	return NULL;
}

char* set_section(int level, char* title, int nelements&; ...)
{
	char start = '';
	char end = '';
	switch (level) {
		case 0:
			start = '[';
			end = ']';
			break;
		case 1:
			start = '{';
			end = '}';
			break;
		case 2:
			start = '(';
			end = ')';
			break;
		case 3:
			start = '';
			end = '';
			break;
		default:
			return -EINVAL;
	}
	int ptr = strlen(title) + 2;
	char* token = (char*)malloc(ptr);
	strcpy(token, title);
	token[ptr - 2] = ':';
	size = 0;
	va_list v;
	va_start(v, nelements);
	while ((char *val = va_arg(v, char*)) != NULL) {
		int len = strlen(val);
		token = (char**)realloc(token, ptr+len+3);
		token[ptr] = start;
		strcpy(&token[ptr+1], val);
		token[ptr+len] = end;
		token[ptr+len+1] = ',';
		size++;
		ptr += len+3;
	}
	va_end(v);
	nelements = size;
	return token;
}
