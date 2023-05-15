#ifndef GETLINE_H
#define GETLINE_H

#include <stdio.h>

#define restrict __restrict

extern ssize_t getline(char **restrict lineptr, size_t *restrict n, FILE *restrict stream);
extern ssize_t getdelim(char **restrict lineptr, size_t *restrict n, int delim, FILE *restrict stream);

#endif /* GETLINE_H */
